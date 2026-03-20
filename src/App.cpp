#include "App.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
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
    // 1. 繪製地圖
    m_ControlStage->DrawMap();

    // 2. 獲取滑鼠座標並偵測懸停 (Hover)
    glm::vec2 mousePos = Util::Input::GetCursorPosition();
    m_HoveredSlot = nullptr; // 每幀重置

    for (auto& slot : m_TowerSlots) {
        slot->Draw(); // 畫出旗子基座

        // 偵測滑鼠是否靠近旗子 (半徑 40 像素)
        float dist = glm::distance(mousePos, slot->m_Transform.translation);
        if (dist < 40.0f) {
            m_HoveredSlot = slot;
        }
    }

    // 3. 處理選單互動與蓋塔
    if (m_HoveredSlot != nullptr) {
        /* 提示：這裡可以呼叫一個畫出 UI 選單圖片的函式
           DrawBuildMenu(m_HoveredSlot->m_Transform.translation);
        */

        // 如果按下左鍵，根據滑鼠相對中心點的位置決定塔種
        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            glm::vec2 offset = mousePos - m_HoveredSlot->m_Transform.translation;
            Tower::Type selectedType;
            bool shouldBuild = false;

            // 判斷方位 (上下左右)
            if (offset.y > 15) {
                selectedType = Tower::Type::ARCHER; // 上：弓箭
                shouldBuild = true;
            } else if (offset.x < -15) {
                selectedType = Tower::Type::MAGE;   // 左：魔法
                shouldBuild = true;
            } else if (offset.x > 15) {
                selectedType = Tower::Type::BOMB;   // 右：炸彈
                shouldBuild = true;
            } else if (offset.y < -15) {
                selectedType = Tower::Type::BARRACKS; // 下：兵營
                shouldBuild = true;
            }

            if (shouldBuild) {
                m_ControlStage->BuildTower(selectedType, m_HoveredSlot->m_Transform.translation);
                LOG_INFO("Tower Built via Menu Selection!");
            }
        }
    }

    // 4. 更新塔與繪製
    m_ControlStage->UpdateTowers(m_Enemies);
    m_ControlStage->DrawTowers();

    // 5. 更新怪物
    for (auto it = m_Enemies.begin(); it != m_Enemies.end(); ) {
        (*it)->Update();
        (*it)->Draw();
        if ((*it)->IsDead()) {
            it = m_Enemies.erase(it);
        } else {
            ++it;
        }
    }

    // 功能按鍵區
    if (Util::Input::IsKeyDown(Util::Keycode::E)) m_ControlStage->SpawnMonster(EnemyFactory::Type::GOBLIN, m_Enemies);
    if (Util::Input::IsKeyDown(Util::Keycode::NUM_1)) ChangeLevel(1);
    if (Util::Input::IsKeyDown(Util::Keycode::NUM_2)) ChangeLevel(2);
    if (Util::Input::IsKeyDown(Util::Keycode::NUM_3)) ChangeLevel(3);
    if (Util::Input::IsKeyDown(Util::Keycode::NUM_4)) ChangeLevel(4);
    if (Util::Input::IsKeyDown(Util::Keycode::NUM_5)) ChangeLevel(5);

    if (Util::Input::IsKeyDown(Util::Keycode::SPACE)) {
        std::cout << "Mouse: {" << mousePos.x << ", " << mousePos.y << "}" << std::endl;
    }

    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }
}

void App::End() {
    LOG_TRACE("End");
}