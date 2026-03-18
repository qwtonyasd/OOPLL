#include "App.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include <iostream>

void App::Start() {
    LOG_TRACE("Start");

    // 1. 初始化關卡大腦 (ControlStage)
    // 它會負責管理 MapManager 並與 EnemyFactory 溝通
    m_ControlStage = std::make_unique<ControlStage>();

    // 2. 預設載入第一關
    ChangeLevel(1);

    m_CurrentState = State::UPDATE;
}

void App::ChangeLevel(int levelId) {
    LOG_INFO("Loading Level: {}", levelId);

    // 1. 叫大腦初始化關卡（包含從 MapFactory 載入地圖、路徑與塔位）
    m_ControlStage->InitLevel(levelId);

    // 2. 切換關卡時，清空舊有的塔位與怪物，避免殘留
    m_TowerSlots.clear();
    m_Enemies.clear();

    // 3. 從大腦獲取當前地圖配置的塔位座標，並生成 TowerSlot 物件
    auto currentMap = m_ControlStage->GetCurrentMap();
    if (currentMap) {
        const auto& slotPositions = currentMap->GetTowerSlots();
        for (const auto& pos : slotPositions) {
            m_TowerSlots.push_back(std::make_shared<TowerSlot>(pos));
        }
    }
}

void App::Update() {
    // --- 繪製與更新區 (由底層往上畫：地圖 -> 塔位 -> 怪物) ---

    // 1. 透過大腦畫出當前關卡地圖 (ZIndex: -10)
    m_ControlStage->DrawMap();

    // 2. 畫出所有塔位 (ZIndex: 5)
    for (auto& slot : m_TowerSlots) {
        slot->Draw();
    }

    // 3. 更新與繪製所有存活的怪物
    // 使用 iterator 迴圈以便在怪物抵達終點時安全地從 vector 中刪除
    for (auto it = m_Enemies.begin(); it != m_Enemies.end(); ) {
        // 更新位移邏輯 (內部呼叫 Unit::MoveTowardsNextNode)
        (*it)->Update();

        // 畫出怪物圖片 (ZIndex: 10)
        (*it)->Draw();

        // 檢查怪物狀態：若抵達終點或 HP <= 0，則從畫面上移除
        if ((*it)->IsDead()) {
            it = m_Enemies.erase(it);
        } else {
            ++it;
        }
    }

    // --- 核心功能：按下鍵盤 E 鍵召喚哥布林 ---
    if (Util::Input::IsKeyDown(Util::Keycode::E)) {
        // 叫大腦從地圖的 4 條路徑中隨機選一條，並透過工廠產出哥布林丟進 m_Enemies
        m_ControlStage->SpawnMonster(EnemyFactory::Type::GOBLIN, m_Enemies);
        LOG_DEBUG("Spawned a Goblin on a random route via ControlStage!");
    }

    // --- 關卡切換邏輯 (數字鍵 1~5) ---
    if (Util::Input::IsKeyDown(Util::Keycode::NUM_1)) ChangeLevel(1);
    if (Util::Input::IsKeyDown(Util::Keycode::NUM_2)) ChangeLevel(2);
    if (Util::Input::IsKeyDown(Util::Keycode::NUM_3)) ChangeLevel(3);
    if (Util::Input::IsKeyDown(Util::Keycode::NUM_4)) ChangeLevel(4);
    if (Util::Input::IsKeyDown(Util::Keycode::NUM_5)) ChangeLevel(5);

    // --- 偵錯功能：按住空白鍵輸出當前滑鼠座標 (輔助您設定路徑點) ---
    if (Util::Input::IsKeyDown(Util::Keycode::SPACE)) {
        glm::vec2 mousePos = Util::Input::GetCursorPosition();
        std::cout << "Coordinate: {" << mousePos.x << ", " << mousePos.y << "}," << std::endl;
    }

    // --- 系統退出處理 ---
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }
}

void App::End() {
    LOG_TRACE("End");
}
