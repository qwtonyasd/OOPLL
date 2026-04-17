#include "VictoryMenu.hpp"
#include "Util/Logger.hpp"

VictoryMenu::VictoryMenu() {
    const std::string basePath = "../PTSD/assets/sprites/images/Start/";

    // 1. 載入你的合成大圖 (18.png)
    m_MainFrame = std::make_shared<Util::GameObject>();
    m_MainFrame->SetDrawable(std::make_shared<Util::Image>(basePath + "18.png"));
    m_MainFrame->m_Transform.translation = {0.0f, 50.0f}; // 整體畫面稍微偏上
    m_MainFrame->SetZIndex(100.0f);

    // 2. 疊加星星 (13.png)
    // 即使你合成了大圖，星星還是分開控制比較好（例如將來要做動態亮起）
    for (int i = 0; i < 3; ++i) {
        auto star = std::make_shared<Util::GameObject>();
        star->SetDrawable(std::make_shared<Util::Image>(basePath + "13.png"));
        // 配合 18.png 的圓孔座標偏移
        float xOffset = (i - 1) * 42.0f;
        star->m_Transform.translation = {xOffset, 135.0f}; // 135 是配合橫幅高度
        star->SetZIndex(110.0f);
        m_Stars.push_back(star);
    }
}

void VictoryMenu::Update() {
    if (!m_Visible) return;

    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
        float cx = m_MainFrame->m_Transform.translation.x;
        float cy = m_MainFrame->m_Transform.translation.y;

        // --- 判定 Continue 按鈕區域 ---
        // 寬度約 180, 高度約 55
        if (IsMouseInsideRect(cx, cy - 65.0f, 180.0f, 55.0f)) {
            LOG_DEBUG("Continue Clicked - Signal Sent");
            m_ContinuePressed = true;
        }
        // --- 判定 Restart 按鈕區域 ---
        else if (IsMouseInsideRect(cx, cy - 140.0f, 180.0f, 55.0f)) {
            LOG_DEBUG("Restart Clicked - Signal Sent");
            m_RestartPressed = true;
        }
    }
}

void VictoryMenu::Draw() {
    if (!m_Visible) return;
    m_MainFrame->Draw();
    for (auto& star : m_Stars) {
        star->Draw();
    }
}

bool VictoryMenu::IsMouseInsideRect(float x, float y, float w, float h) {
    glm::vec2 mousePos = Util::Input::GetCursorPosition();
    return (mousePos.x >= x - w/2 && mousePos.x <= x + w/2 &&
            mousePos.y >= y - h/2 && mousePos.y <= y + h/2);
}