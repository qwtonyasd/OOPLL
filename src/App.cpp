#include "App.hpp"
#include "MapFactory.hpp"
#include "EnemyFactory.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "GameManager.hpp"
#include <algorithm>
#include <set>

// 建立一個全域或成員變數，用來追蹤哪些子彈已經被加入過 m_Root
// 這樣可以避免每一幀重複 AddChild 導致的效能與渲染問題
static std::set<std::shared_ptr<Projectile>> g_ManagedProjectiles;

void App::Start() {
    LOG_TRACE("App Started");
    m_MapManager = std::make_unique<MapManager>();
    m_WorldMap = std::make_unique<WorldMap>();

    // 將 m_Root 傳入，讓塔能被正確管理
    m_TowerManager = std::make_unique<TowerManager>(m_Root);

    m_BuildMenu = std::make_unique<BuildMenu>();
    m_Hud = std::make_unique<Hud>();

    m_IsInGame = false;
    m_CurrentState = State::UPDATE;
}

void App::Update() {
    if (m_IsInGame) {
        HandleGamePlay();
    } else {
        HandleSelectLevel();
    }

    if (Util::Input::IsKeyDown(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }
}

void App::HandleSelectLevel() {
    if (m_WorldMap) {
        int selectedLevel = m_WorldMap->Update(3);
        if (selectedLevel > 0) {
            ChangeLevel(selectedLevel);
            m_IsInGame = true;
        }
    }
}

void App::HandleGamePlay() {
    // 1. 繪製地圖底層
    m_MapManager->Draw();

    glm::vec2 mousePos = Util::Input::GetCursorPosition();
    auto currentMap = m_MapManager->GetCurrentMap();

    // ---------------------------------------------------------
    // 2. 生成怪物 Debug
    // ---------------------------------------------------------
    if (currentMap && !currentMap->GetRoutes().empty()) {
        const auto& mainRoute = currentMap->GetRoutes()[0];
        if (Util::Input::IsKeyDown(Util::Keycode::E)) {
            m_Enemies.push_back(EnemyFactory::Create(Enemy::Type::GOBLIN, mainRoute));
        }
        if (Util::Input::IsKeyDown(Util::Keycode::R)) {
            m_Enemies.push_back(EnemyFactory::Create(Enemy::Type::ORC, mainRoute));
        }
    }

    // ---------------------------------------------------------
    // 3. 蓋塔邏輯
    // ---------------------------------------------------------
    if (m_BuildMenu->IsVisible()) {
        m_BuildMenu->Draw();
        Tower::Type selectedType = m_BuildMenu->Update();

        if (selectedType != Tower::Type::NONE) {
            if (m_SelectedSlot) {
                int cost = Tower::GetBaseCost(selectedType);
                if (GameManager::GetInstance().SpendMoney(cost)) {
                    std::vector<glm::vec2> route = (currentMap && !currentMap->GetRoutes().empty())
                                                   ? currentMap->GetRoutes()[0] : std::vector<glm::vec2>{};
                    m_TowerManager->AddTower(selectedType, m_SelectedSlot->GetPosition(), route);
                    m_SelectedSlot->SetOccupied(true);
                }
            }
            m_BuildMenu->SetVisible(false);
            m_SelectedSlot = nullptr;
            return;
        }

        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            float distToMenu = glm::distance(mousePos, m_BuildMenu->GetTransform().translation);
            if (distToMenu > 120.0f) {
                m_BuildMenu->SetVisible(false);
                m_SelectedSlot = nullptr;
            }
        }
    } else {
        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            for (auto& slot : m_TowerSlots) {
                if (glm::distance(mousePos, slot->GetPosition()) < 40.0f && !slot->IsOccupied()) {
                    m_SelectedSlot = slot;
                    m_BuildMenu->SetPosition(slot->GetPosition());
                    m_BuildMenu->SetVisible(true);
                    break;
                }
            }
        }
    }

    for (auto& slot : m_TowerSlots) slot->Draw();

    // ---------------------------------------------------------
    // 4. 核心：塔與子彈更新 (渲染樹修正)
    // ---------------------------------------------------------

    // 更新塔 (會呼叫 Attack 並產生子彈放入 m_Projectiles)
    m_TowerManager->UpdateAll(m_Enemies, m_Projectiles);
    m_TowerManager->DrawAll();

    // 更新與管理子彈
    for (auto it = m_Projectiles.begin(); it != m_Projectiles.end(); ) {
        auto& p = *it;

        // --- 修正：不將子彈加入 m_Root，直接手動管理生命週期與繪製 ---
        // 這樣可以避免 p->Draw() 與 m_Root 自動繪製衝突，也能保證使用絕對座標

        p->Update();
        p->Draw(); // 這裡執行 Projectile::Draw()

        if (!p->IsActive()) {
            // 如果你之前有 AddChild，這裡才需要 RemoveChild
            // m_Root.RemoveChild(p);
            it = m_Projectiles.erase(it);
        } else {
            ++it;
        }
    }

    // ---------------------------------------------------------
    // 5. 敵人更新
    // ---------------------------------------------------------
    for (auto it = m_Enemies.begin(); it != m_Enemies.end(); ) {
        auto& enemy = *it;
        enemy->Update();
        enemy->Draw();

        bool shouldRemove = false;
        if (enemy->ReachedEnd()) {
            GameManager::GetInstance().TakeDamage(1);
            shouldRemove = true;
        } else if (enemy->GetHP() <= 0 && enemy->IsDeadAnimationFinished()) {
            int reward = (enemy->GetType() == Enemy::Type::GOBLIN) ? 3 : 9;
            GameManager::GetInstance().AddMoney(reward);
            shouldRemove = true;
        }

        if (shouldRemove) {
            it = m_Enemies.erase(it);
        } else {
            ++it;
        }
    }

    if (m_Hud) m_Hud->Draw();

    if (Util::Input::IsKeyDown(Util::Keycode::BACKSPACE)) {
        m_IsInGame = false;
    }
}

void App::ChangeLevel(int levelId) {
    auto newMap = MapFactory::CreateLevel(levelId);
    m_MapManager->AddLevel(levelId, newMap);
    m_MapManager->SwitchLevel(levelId);

    GameManager::GetInstance().InitLevel(265, 20);

    m_TowerManager->Clear();

    // 清理子彈與渲染樹管理容器
    for (auto& p : m_Projectiles) {
        m_Root.RemoveChild(p);
    }
    m_Projectiles.clear();
    g_ManagedProjectiles.clear(); // 換關必清

    m_TowerSlots.clear();
    m_Enemies.clear();

    auto currentMap = m_MapManager->GetCurrentMap();
    if (currentMap) {
        for (const auto& pos : currentMap->GetTowerSlots()) {
            m_TowerSlots.push_back(std::make_shared<TowerSlot>(pos));
        }
    }

    m_BuildMenu->SetVisible(false);
}

void App::End() {
    LOG_TRACE("App Terminated");
}