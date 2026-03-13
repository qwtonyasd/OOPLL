#include "App.hpp"
#include "MapFactory.hpp"  // 引入工廠
#include "TowerSlot.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include <iostream>

void App::Start() {
    LOG_TRACE("Start");

    // 1. 初始化 Manager
    m_MapManager = std::make_unique<MapManager>();

    // 2. 初始載入第一關 (這會自動建立地圖與塔位)
    ChangeLevel(1);

    m_CurrentState = State::UPDATE;
}

void App::ChangeLevel(int levelId) {
    LOG_INFO("Loading Level: {}", levelId);

    // 1. 叫 MapManager 透過工廠載入新地圖物件
    m_MapManager->LoadLevel(levelId);

    // 2. 清除舊有的塔位物件
    m_TowerSlots.clear();

    // 3. 從當前地圖取得新座標，並生成對應的 TowerSlot 物件
    auto currentMap = m_MapManager->GetCurrentMap();
    if (currentMap) {
        const auto& slotPositions = currentMap->GetTowerSlots();
        for (const auto& pos : slotPositions) {
            m_TowerSlots.push_back(std::make_shared<TowerSlot>(pos));
        }
    }
}

void App::Update() {
    // --- 繪製區區 (由底層往上畫) ---

    // 1. 畫地圖 (最底層，ZIndex 通常為負)
    m_MapManager->Draw();

    // 2. 畫塔位 (疊在地圖上方)
    for (auto& slot : m_TowerSlots) {
        slot->Draw();
    }

    // --- 關卡切換邏輯 (範例：按下數字鍵 1 或 2) ---
    if (Util::Input::IsKeyDown(Util::Keycode::NUM_1)) {
        ChangeLevel(1);
    }
    if (Util::Input::IsKeyDown(Util::Keycode::NUM_2)) {
        ChangeLevel(2);
    }
    if (Util::Input::IsKeyDown(Util::Keycode::NUM_3)) {
        ChangeLevel(3);
    }
    if (Util::Input::IsKeyDown(Util::Keycode::NUM_4)) {
        ChangeLevel(4);
    }
    if (Util::Input::IsKeyDown(Util::Keycode::NUM_5)) {
        ChangeLevel(5);
    }

    // --- 偵錯功能：按住空白鍵記錄座標 (方便你設計新地圖) ---
    if (Util::Input::IsKeyDown(Util::Keycode::SPACE)) {
        glm::vec2 mousePos = Util::Input::GetCursorPosition();
        std::cout << "Coordinate: {" << mousePos.x << ", " << mousePos.y << "}," << std::endl;
    }

    // --- 結束處理 ---
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }
}

void App::End() {
    LOG_TRACE("End");
}