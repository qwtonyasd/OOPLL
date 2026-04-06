#include "App.hpp"
#include "MapFactory.hpp"
#include "EnemyFactory.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "GameManager.hpp"
#include "Hud.hpp"
#include <algorithm>

void App::Start() {
    LOG_TRACE("App Started");
    m_MapManager = std::make_unique<MapManager>();
    m_WorldMap = std::make_unique<WorldMap>();
    m_TowerManager = std::make_unique<TowerManager>();
    m_BuildMenu = std::make_unique<BuildMenu>();
    m_Hud = std::make_unique<Hud>();

    m_IsInGame = false;
    m_CurrentState = State::UPDATE;
    m_SelectedSlot = nullptr;
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
    m_MapManager->Draw();
    glm::vec2 mousePos = Util::Input::GetCursorPosition();

    // 1. 處理塔位點擊與繪製
    for (auto& slot : m_TowerSlots) {
        slot->Draw();
        // 偵測是否點擊該塔位
        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            if (glm::distance(mousePos, slot->GetPosition()) < 40.0f && !slot->IsOccupied()) {
                m_SelectedSlot = slot;
                m_BuildMenu->SetPosition(slot->GetPosition());
                m_BuildMenu->SetVisible(true);
                LOG_DEBUG("Build Menu Opened at: {}, {}", slot->GetPosition().x, slot->GetPosition().y);
            }
        }
    }

    // 2. 處理蓋塔選單與小兵路徑邏輯
    if (m_BuildMenu->IsVisible()) {
        m_BuildMenu->Draw();
        Tower::Type selectedType = m_BuildMenu->Update();

        if (selectedType != Tower::Type::NONE && m_SelectedSlot) {
            int cost = Tower::GetBaseCost(selectedType);

            if (GameManager::GetInstance().SpendMoney(cost)) {
                // --- 重要：獲取路徑資訊以供兵營使用 ---
                auto currentMap = m_MapManager->GetCurrentMap();
                std::vector<glm::vec2> selectedRoute;

                if (currentMap) {
                    // 取得該地圖的第一條主要路徑 (或是你可以實作 GetNearestRoute)
                    const auto& allRoutes = currentMap->GetRoutes();
                    if (!allRoutes.empty()) {
                        selectedRoute = allRoutes[0];
                    }
                }

                // 將路徑傳給 TowerManager，讓兵營知道要把小兵生在哪
                m_TowerManager->AddTower(selectedType, m_SelectedSlot->GetPosition(), selectedRoute);

                m_SelectedSlot->SetOccupied(true);
                m_BuildMenu->SetVisible(false);
                m_SelectedSlot = nullptr;
                LOG_INFO("Tower built successfully!");
            } else {
                LOG_INFO("Not enough money to build!");
                m_BuildMenu->SetVisible(false); // 錢不夠就關閉選單
            }
        }
    }

    // 3. 更新與繪製所有戰鬥單位 (塔與敵人)
    m_TowerManager->UpdateAll(m_Enemies);
    m_TowerManager->DrawAll();

    for (auto it = m_Enemies.begin(); it != m_Enemies.end(); ) {
        (*it)->Update();
        (*it)->Draw();

        if ((*it)->GetHP() <= 0) {
            (*it)->OnDeath();
            it = m_Enemies.erase(it);
        } else if ((*it)->ReachedEnd()) {
            GameManager::GetInstance().ReduceHealth(1);
            it = m_Enemies.erase(it);
        } else {
            ++it;
        }
    }

    // 4. 繪製 UI (血量、金錢)
    if (m_Hud) m_Hud->Draw();

    // 5. 測試功能與返回
    // 按 E 生成敵人
    if (Util::Input::IsKeyDown(Util::Keycode::E)) {
        auto currentMap = m_MapManager->GetCurrentMap();
        if (currentMap) {
            const auto& route = currentMap->GetRandomRoute();
            m_Enemies.push_back(EnemyFactory::Create(EnemyFactory::Type::GOBLIN, route));
        }
    }

    // 按 Backspace 返回世界地圖
    if (Util::Input::IsKeyDown(Util::Keycode::BACKSPACE)) {
        m_IsInGame = false;
    }
}

void App::ChangeLevel(int levelId) {
    auto newMap = MapFactory::CreateLevel(levelId);
    m_MapManager->AddLevel(levelId, newMap);
    m_MapManager->SwitchLevel(levelId);

    // 初始化該關卡的資源
    GameManager::GetInstance().InitLevel(265, 20);

    m_TowerManager->Clear();
    m_TowerSlots.clear();
    m_Enemies.clear();
    m_BuildMenu->SetVisible(false);

    auto currentMap = m_MapManager->GetCurrentMap();
    if (currentMap) {
        for (const auto& pos : currentMap->GetTowerSlots()) {
            m_TowerSlots.push_back(std::make_shared<TowerSlot>(pos));
        }
    }
}

void App::End() {
    LOG_TRACE("App Terminated");
}