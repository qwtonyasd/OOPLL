#include "App.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Tower.hpp"
#include <iostream>

void App::Start() {
    LOG_TRACE("Start");
    m_ControlStage = std::make_unique<ControlStage>();
    ChangeLevel(1);
    m_CurrentState = State::UPDATE;
}

void App::ChangeLevel(int levelId) {
    LOG_INFO("Loading Level: {}", levelId);
    m_ControlStage->InitLevel(levelId);

    m_TowerSlots.clear();
    m_Enemies.clear();

    auto currentMap = m_ControlStage->GetCurrentMap();
    if (currentMap) {
        const auto& slotPositions = currentMap->GetTowerSlots();
        for (const auto& pos : slotPositions) {
            m_TowerSlots.push_back(std::make_shared<TowerSlot>(pos));
        }
    }
}

void App::Update() {
    // 1. 繪製地圖 (最底層)
    m_ControlStage->DrawMap();

    // 2. 處理蓋塔輸入
    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
        glm::vec2 mousePos = Util::Input::GetCursorPosition();
        for (auto& slot : m_TowerSlots) {
            if (glm::distance(mousePos, slot->m_Transform.translation) < 30.0f) {
                // 現在 ControlStage 已經有 BuildTower 了！
                m_ControlStage->BuildTower(Tower::Type::ARCHER, slot->m_Transform.translation);
                LOG_INFO("Tower Built!");
                break;
            }
        }
    }

    // 3. 繪製塔位
    for (auto& slot : m_TowerSlots) {
        slot->Draw();
    }

    // 4. 更新與繪製塔
    m_ControlStage->UpdateTowers(m_Enemies);
    m_ControlStage->DrawTowers();

    // 5. 更新與繪製怪物
    for (auto it = m_Enemies.begin(); it != m_Enemies.end(); ) {
        (*it)->Update();
        (*it)->Draw();
        if ((*it)->IsDead()) {
            it = m_Enemies.erase(it);
        } else {
            ++it;
        }
    }

    // 快速召喚與關卡切換
    if (Util::Input::IsKeyDown(Util::Keycode::E)) m_ControlStage->SpawnMonster(EnemyFactory::Type::GOBLIN, m_Enemies);
    if (Util::Input::IsKeyDown(Util::Keycode::NUM_1)) ChangeLevel(1);
    if (Util::Input::IsKeyDown(Util::Keycode::NUM_2)) ChangeLevel(2);
    if (Util::Input::IsKeyDown(Util::Keycode::NUM_3)) ChangeLevel(3);
    if (Util::Input::IsKeyDown(Util::Keycode::NUM_4)) ChangeLevel(4);
    if (Util::Input::IsKeyDown(Util::Keycode::NUM_5)) ChangeLevel(5);

    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }
}

void App::End() {
    LOG_TRACE("End");
}