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

    // 1. 處理塔位點擊
    for (auto& slot : m_TowerSlots) {
        slot->Draw();
        // 修正：使用 IsKeyDown 偵測點擊
        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            if (glm::distance(mousePos, slot->GetPosition()) < 40.0f && !slot->IsOccupied()) {
                m_SelectedSlot = slot;
                m_BuildMenu->SetPosition(slot->GetPosition());
                m_BuildMenu->SetVisible(true);
                LOG_DEBUG("Build Menu Opened at: {}, {}", slot->GetPosition().x, slot->GetPosition().y);
            }
        }
    }

    // 2. 處理蓋塔選單
    if (m_BuildMenu->IsVisible()) {
        m_BuildMenu->Draw();
        // 修正：接收 Tower::Type
        Tower::Type selectedType = m_BuildMenu->Update();

        if (selectedType != Tower::Type::NONE && m_SelectedSlot) {
            int cost = Tower::GetBaseCost(selectedType);
            if (GameManager::GetInstance().SpendMoney(cost)) {
                m_TowerManager->AddTower(selectedType, m_SelectedSlot->GetPosition());
                m_SelectedSlot->SetOccupied(true);
                m_BuildMenu->SetVisible(false);
                m_SelectedSlot = nullptr;
            }
        }
    }

    // 3. 更新與繪製戰鬥單位
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

    if (m_Hud) m_Hud->Draw();

    // 熱鍵功能
    if (Util::Input::IsKeyDown(Util::Keycode::E)) {
        auto currentMap = m_MapManager->GetCurrentMap();
        if (currentMap) {
            const auto& route = currentMap->GetRandomRoute();
            m_Enemies.push_back(EnemyFactory::Create(EnemyFactory::Type::GOBLIN, route));
        }
    }

    if (Util::Input::IsKeyDown(Util::Keycode::BACKSPACE)) {
        m_IsInGame = false;
    }
}

void App::ChangeLevel(int levelId) {
    auto newMap = MapFactory::CreateLevel(levelId);
    m_MapManager->AddLevel(levelId, newMap);
    m_MapManager->SwitchLevel(levelId);

    GameManager::GetInstance().InitLevel(200, 20);

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