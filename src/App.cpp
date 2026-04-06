#include "App.hpp"
#include "MapFactory.hpp"
#include "EnemyFactory.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "GameManager.hpp"

void App::Start() {
    LOG_TRACE("App Started");
    m_MapManager = std::make_unique<MapManager>();
    m_WorldMap = std::make_unique<WorldMap>();
    m_TowerManager = std::make_unique<TowerManager>();
    m_BuildMenu = std::make_unique<BuildMenu>();

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
        // 修正點擊旗幟功能：捕捉回傳的關卡 ID
        int selectedLevel = m_WorldMap->Update(3);

        if (selectedLevel > 0) {
            LOG_INFO("Selected Level: {}", selectedLevel);
            ChangeLevel(selectedLevel);
            m_IsInGame = true;
        }
    }
}

void App::HandleGamePlay() {
    m_MapManager->Draw();
    glm::vec2 mousePos = Util::Input::GetCursorPosition();

    for (auto& slot : m_TowerSlots) {
        slot->Draw();
    }

    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
        if (m_BuildMenu->IsVisible()) {
            Tower::Type type = m_BuildMenu->GetSelectedType();
            if (type != Tower::Type::NONE) {
                int cost = Tower::GetBaseCost(type);
                if (GameManager::GetInstance().SpendMoney(cost)) {
                    m_TowerManager->AddTower(type, m_ActiveSlot->GetTransform().translation);
                    m_ActiveSlot->SetOccupied(true);
                }
                m_BuildMenu->Hide();
                m_ActiveSlot = nullptr;
            } else {
                m_BuildMenu->Hide();
                m_ActiveSlot = nullptr;
            }
        }
        else {
            for (auto& slot : m_TowerSlots) {
                glm::vec2 slotPos = slot->GetTransform().translation;
                if (!slot->IsOccupied() && glm::distance(mousePos, slotPos) < 40.0f) {
                    m_ActiveSlot = slot;
                    m_BuildMenu->Show(slotPos);
                    break;
                }
            }
        }
    }

    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_RB)) {
        m_BuildMenu->Hide();
        m_ActiveSlot = nullptr;
    }

    m_TowerManager->UpdateAll(m_Enemies);
    m_TowerManager->DrawAll();

    if (m_BuildMenu->IsVisible()) {
        m_BuildMenu->Draw();
    }

    for (auto it = m_Enemies.begin(); it != m_Enemies.end(); ) {
        (*it)->Update();
        (*it)->Draw();
        if ((*it)->GetHP() <= 0) {
            (*it)->OnDeath();
            it = m_Enemies.erase(it);
        } else {
            ++it;
        }
    }

    if (Util::Input::IsKeyDown(Util::Keycode::E)) {
        auto currentMap = m_MapManager->GetCurrentMap();
        if (currentMap) {
            const auto& route = currentMap->GetRandomRoute();
            m_Enemies.push_back(EnemyFactory::Create(EnemyFactory::Type::GOBLIN, route));
        }
    }

    if (Util::Input::IsKeyDown(Util::Keycode::BACKSPACE)) {
        m_IsInGame = false;
        m_BuildMenu->Hide();
    }
}

void App::ChangeLevel(int levelId) {
    auto newMap = MapFactory::CreateLevel(levelId);
    m_MapManager->AddLevel(levelId, newMap);
    m_MapManager->SwitchLevel(levelId);

    GameManager::GetInstance().InitLevel(265, 20);

    m_TowerManager->Clear();
    m_TowerSlots.clear();
    m_Enemies.clear();

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