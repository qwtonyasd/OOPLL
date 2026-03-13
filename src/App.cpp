#include "App.hpp"
#include "Map.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include <iostream> // 確保有這行

void App::Start() {
    LOG_TRACE("Start");

    m_MapManager = std::make_unique<MapManager>();

    // 這裡換成我幫你預估的 Kingdom Rush 地圖座標，你可以先跑跑看準不準
    std::vector<glm::vec2> lv1Path = {
        { -15.0f,   450.0f},  // 起點 (上方)
        { -15.0f,   180.0f},  // 彎道1
        {-245.0f,    10.0f},  // 彎道2 (左側)
        {-100.0f,  -200.0f},  // 彎道3
        { 250.0f,  -140.0f},  // 經過村莊
        { 550.0f,  -140.0f}   // 終點 (右側)
    };

    auto level1 = std::make_shared<Map>("../PTSD/assets/sprites/images/287.png", lv1Path);

    m_MapManager->AddLevel(1, level1);
    m_MapManager->SwitchLevel(1);

    m_CurrentState = State::UPDATE;
}

void App::Update() {
    m_MapManager->Draw();

    // === 偵錯工具：加在這裡 ===
    // 取得目前滑鼠座標
    glm::vec2 mousePos = Util::Input::GetCursorPosition();

    // 做法 A：如果 Console 沒反應，我們直接按空白鍵時強迫印出
    if (Util::Input::IsKeyDown(Util::Keycode::SPACE)) {
        std::cout << "Coords: {" << mousePos.x << ", " << mousePos.y << "}," << std::endl;
        LOG_INFO("Mouse at: {}, {}", mousePos.x, mousePos.y);
    }

    /* 做法 B (最強招)：直接把座標顯示在視窗標題列
     * 這樣你連看 Console 都不用，滑鼠移到哪，標題就會顯示到哪
     */
    // 注意：這裡假設你的框架有提供設定標題的功能，若編譯不過可刪除此行
    // SDL_SetWindowTitle(m_Context->GetWindow(), ("Pos: " + std::to_string(mousePos.x) + ", " + std::to_string(mousePos.y)).c_str());

    // 結束判斷
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }
}

void App::End() {
    LOG_TRACE("End");
}