#include "App.hpp"
#include "MapFactory.hpp"
#include "EnemyFactory.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "GameManager.hpp"

void App::Start() {
    LOG_TRACE("App Started");

    // 初始化核心系統
    m_MapManager = std::make_unique<MapManager>();
    m_WorldMap = std::make_unique<WorldMap>();

    // 將 m_Root 傳入 TowerManager，使其產生的塔能自動加入渲染樹
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

    // 全局退出偵測
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
    // 1. 底層繪製：地圖
    m_MapManager->Draw();

    glm::vec2 mousePos = Util::Input::GetCursorPosition();
    auto currentMap = m_MapManager->GetCurrentMap();

    // ---------------------------------------------------------
    // 新增：偵測 E 和 R 鍵手動生成怪物 (Debug 功能)
    // ---------------------------------------------------------
    if (currentMap && !currentMap->GetRoutes().empty()) {
        const auto& mainRoute = currentMap->GetRoutes()[0];

        if (Util::Input::IsKeyDown(Util::Keycode::E)) {
            auto goblin = EnemyFactory::Create(Enemy::Type::GOBLIN, mainRoute);
            if (goblin) {
                m_Enemies.push_back(goblin);
                LOG_INFO("Spawned Goblin via E key");
            }
        }

        if (Util::Input::IsKeyDown(Util::Keycode::R)) {
            auto orc = EnemyFactory::Create(Enemy::Type::ORC, mainRoute);
            if (orc) {
                m_Enemies.push_back(orc);
                LOG_INFO("Spawned Orc via R key");
            }
        }
    }

    // 2. 處理蓋塔選單
    if (m_BuildMenu->IsVisible()) {
        m_BuildMenu->Draw();

        Tower::Type selectedType = m_BuildMenu->Update();

        if (selectedType != Tower::Type::NONE) {
            if (m_SelectedSlot) {
                int cost = Tower::GetBaseCost(selectedType);
                if (GameManager::GetInstance().SpendMoney(cost)) {
                    // 取得地圖路徑供兵營使用
                    std::vector<glm::vec2> route = (currentMap && !currentMap->GetRoutes().empty())
                                                   ? currentMap->GetRoutes()[0] : std::vector<glm::vec2>{};

                    m_TowerManager->AddTower(selectedType, m_SelectedSlot->GetPosition(), route);
                    m_SelectedSlot->SetOccupied(true);
                } else {
                    LOG_DEBUG("Money not enough!");
                }
            }
            m_BuildMenu->SetVisible(false);
            m_SelectedSlot = nullptr;
            return; // 結束本影格防止滑鼠事件向下傳遞
        }

        // 點擊選單外部關閉選單
        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            float distToMenu = glm::distance(mousePos, m_BuildMenu->GetTransform().translation);
            if (distToMenu > 100.0f) {
                m_BuildMenu->SetVisible(false);
                m_SelectedSlot = nullptr;
                return;
            }
        }
    }
    // 3. 偵測塔位點擊 (僅在選單關閉時執行)
    else {
        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            for (auto& slot : m_TowerSlots) {
                if (glm::distance(mousePos, slot->GetPosition()) < 40.0f && !slot->IsOccupied()) {
                    m_SelectedSlot = slot;
                    m_BuildMenu->SetPosition(slot->GetPosition());
                    m_BuildMenu->SetVisible(true);
                    return; // 重要：開啟選單後立刻結束本影格，防止秒蓋塔
                }
            }
        }
    }

    // 4. 渲染旗幟
    for (auto& slot : m_TowerSlots) {
        slot->Draw();
    }

    // 5. 更新與渲染塔
    for (auto& enemy : m_Enemies) enemy->SetBlocked(false);
    m_TowerManager->UpdateAll(m_Enemies);
    m_TowerManager->DrawAll();

    // 6. 更新與渲染敵人
    for (auto it = m_Enemies.begin(); it != m_Enemies.end(); ) {
        (*it)->Update();
        (*it)->Draw();

        bool shouldRemove = false;
        if ((*it)->ReachedEnd()) {
            GameManager::GetInstance().TakeDamage(1);
            shouldRemove = true;
        } else if ((*it)->GetHP() <= 0 && (*it)->IsDeadAnimationFinished()) {
            shouldRemove = true;
        }

        if (shouldRemove) {
            it = m_Enemies.erase(it);
        } else {
            ++it;
        }
    }

    // 7. 繪製 HUD
    if (m_Hud) m_Hud->Draw();

    // 回地圖
    if (Util::Input::IsKeyDown(Util::Keycode::BACKSPACE)) {
        m_IsInGame = false;
    }
}

void App::ChangeLevel(int levelId) {
    auto newMap = MapFactory::CreateLevel(levelId);
    m_MapManager->AddLevel(levelId, newMap);
    m_MapManager->SwitchLevel(levelId);

    GameManager::GetInstance().InitLevel(500, 20);

    m_TowerManager->Clear();
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