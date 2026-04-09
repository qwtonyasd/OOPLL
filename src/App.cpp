#include "App.hpp"
#include "MapFactory.hpp"
#include "EnemyFactory.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "GameManager.hpp"
#include <algorithm>

void App::Start() {
    LOG_TRACE("App Started");

    // 初始化核心系統
    m_MapManager = std::make_unique<MapManager>();
    m_WorldMap = std::make_unique<WorldMap>();

    // 將渲染樹根節點傳入 TowerManager
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

    // 全局退出
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
    // Debug 生成怪物 (E: Goblin, R: Orc)
    // ---------------------------------------------------------
    if (currentMap && !currentMap->GetRoutes().empty()) {
        const auto& mainRoute = currentMap->GetRoutes()[0];

        if (Util::Input::IsKeyDown(Util::Keycode::E)) {
            m_Enemies.push_back(EnemyFactory::Create(Enemy::Type::GOBLIN, mainRoute));
            LOG_INFO("Spawned Goblin via E key");
        }

        if (Util::Input::IsKeyDown(Util::Keycode::R)) {
            m_Enemies.push_back(EnemyFactory::Create(Enemy::Type::ORC, mainRoute));
            LOG_INFO("Spawned Orc via R key");
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
            return;
        }

        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            float distToMenu = glm::distance(mousePos, m_BuildMenu->GetTransform().translation);
            if (distToMenu > 120.0f) {
                m_BuildMenu->SetVisible(false);
                m_SelectedSlot = nullptr;
                return;
            }
        }
    }
    // 3. 偵測塔位點擊
    else {
        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            for (auto& slot : m_TowerSlots) {
                if (glm::distance(mousePos, slot->GetPosition()) < 40.0f && !slot->IsOccupied()) {
                    m_SelectedSlot = slot;
                    m_BuildMenu->SetPosition(slot->GetPosition());
                    m_BuildMenu->SetVisible(true);
                    return;
                }
            }
        }
    }

    // 4. 繪製塔位旗幟
    for (auto& slot : m_TowerSlots) {
        slot->Draw();
    }

    // ---------------------------------------------------------
    // 5. 核心邏輯：士兵攔截判定與塔更新
    // ---------------------------------------------------------

    // 每一幀先清除敵人的攔截狀態，讓士兵重新決定攔截誰
    for (auto& enemy : m_Enemies) {
        enemy->SetBlocked(false);
    }

    // 更新塔 (會驅動兵營的小兵去尋找並截停敵人)
    m_TowerManager->UpdateAll(m_Enemies);
    m_TowerManager->DrawAll();

    // 6. 更新、渲染與清理敵人
    for (auto it = m_Enemies.begin(); it != m_Enemies.end(); ) {
        auto& enemy = *it;

        enemy->Update();
        enemy->Draw();

        bool shouldRemove = false;

        // 漏怪處理
        if (enemy->ReachedEnd()) {
            GameManager::GetInstance().TakeDamage(1);
            shouldRemove = true;
        }
        // 擊殺處理
        else if (enemy->GetHP() <= 0 && enemy->IsDeadAnimationFinished()) {
            // 根據類型給予金錢：哥布林 3 元，獸人 9 元
            int reward = (enemy->GetType() == Enemy::Type::GOBLIN) ? 3 : 9;
            GameManager::GetInstance().AddMoney(reward);
            LOG_INFO("Enemy Killed! Reward: {} Gold", reward);

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

    // 返回選關介面
    if (Util::Input::IsKeyDown(Util::Keycode::BACKSPACE)) {
        m_IsInGame = false;
    }
}

void App::ChangeLevel(int levelId) {
    auto newMap = MapFactory::CreateLevel(levelId);
    m_MapManager->AddLevel(levelId, newMap);
    m_MapManager->SwitchLevel(levelId);

    // 重點：初始金額設定為 265
    GameManager::GetInstance().InitLevel(265, 20);

    // 清理舊有關卡物件
    m_TowerManager->Clear();
    m_TowerSlots.clear();
    m_Enemies.clear();

    // 生成塔位
    auto currentMap = m_MapManager->GetCurrentMap();
    if (currentMap) {
        for (const auto& pos : currentMap->GetTowerSlots()) {
            m_TowerSlots.push_back(std::make_shared<TowerSlot>(pos));
        }
    }

    m_BuildMenu->SetVisible(false);
    LOG_INFO("Level {} Loaded. Start Gold: 265", levelId);
}

void App::End() {
    LOG_TRACE("App Terminated");
}