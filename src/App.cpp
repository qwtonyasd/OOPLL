#include "App.hpp"
#include "Map.hpp"
#include "TowerSlot.hpp" // 記得檢查這行
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include <iostream>

void App::Start() {
    LOG_TRACE("Start");

    // 1. 先初始化基礎 Manager
    m_MapManager = std::make_unique<MapManager>();

    // 2. 初始化塔位 (這只是把物件裝進 vector，還沒畫出來)
    // 嘗試將座標數值縮小，讓它們往中心靠攏
   // 根據最後截圖微調的相對位置
    m_TowerSlots.push_back(std::make_shared<TowerSlot>(glm::vec2{-110, 110}));   // 頂端路口
    m_TowerSlots.push_back(std::make_shared<TowerSlot>(glm::vec2{-100, 50}));   // 左上彎道內側
    m_TowerSlots.push_back(std::make_shared<TowerSlot>(glm::vec2{-80, -50}));   // U彎左側
    m_TowerSlots.push_back(std::make_shared<TowerSlot>(glm::vec2{30, -50}));    // U彎右側
    m_TowerSlots.push_back(std::make_shared<TowerSlot>(glm::vec2{130, -100}));  // 下方橫向路邊
    m_TowerSlots.push_back(std::make_shared<TowerSlot>(glm::vec2{0, 20}));      // 畫面中心附近的小土堆
    // 3. 設定地圖路徑
    std::vector<glm::vec2> lv1Path = {
        { -15.0f,   450.0f},
        { -15.0f,   180.0f},
        {-245.0f,    10.0f},
        {-100.0f,  -200.0f},
        { 250.0f,  -140.0f},
        { 550.0f,  -140.0f}
    };

    auto level1 = std::make_shared<Map>("../PTSD/assets/sprites/images/287.png", lv1Path);
    m_MapManager->AddLevel(1, level1);
    m_MapManager->SwitchLevel(1);

    m_CurrentState = State::UPDATE;
}

void App::Update() {
    // --- 繪製區區 (每一幀都會跑) ---

    // 1. 先畫地圖 (最底層)
    m_MapManager->Draw();

    // 2. 再畫塔位 (疊在上面)
    for (auto& slot : m_TowerSlots) {
        slot->Draw();
    }

    // --- 偵錯與輸入處理 ---
    glm::vec2 mousePos = Util::Input::GetCursorPosition();

    if (Util::Input::IsKeyDown(Util::Keycode::SPACE)) {
        std::cout << "Coords: {" << mousePos.x << ", " << mousePos.y << "}," << std::endl;
        LOG_INFO("Mouse at: {}, {}", mousePos.x, mousePos.y);
    }

    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }
}

void App::End() {
    LOG_TRACE("End");
}