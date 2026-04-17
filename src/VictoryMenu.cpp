#include "VictoryMenu.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Logger.hpp"

VictoryMenu::VictoryMenu() {
    const std::string basePath = "../PTSD/assets/sprites/images/Start/";

    m_MainFrame = std::make_shared<Util::GameObject>();
    m_MainFrame->SetDrawable(std::make_shared<Util::Image>(basePath + "18.png"));
    m_MainFrame->m_Transform.translation = {0.0f, 50.0f};
    m_MainFrame->SetZIndex(100.0f); // 背景層
    m_MainFrame->SetVisible(false);

    for (int i = 0; i < 3; ++i) {
        auto star = std::make_shared<Util::GameObject>();
        star->SetDrawable(std::make_shared<Util::Image>(basePath + "13.png"));

        // 相對於 18.png 中心點的偏移
        float xOffset = (static_cast<float>(i) - 1.0f) * 60.0f;
        star->m_Transform.translation = {xOffset, 135.0f};

        // 強制星星 ZIndex 高於背景
        star->SetZIndex(250.0f);
        star->SetVisible(false);

        // 建立父子關係
        m_MainFrame->AddChild(star);
        m_Stars.push_back(star);
    }
}

void VictoryMenu::SetVisible(bool visible, int currentHP) {
    m_Visible = visible;
    m_MainFrame->SetVisible(visible);

    if (visible) {
        ResetFlags();
        // 根據血量決定顯示幾顆星星
        int starsCount = (currentHP >= 20) ? 3 : (currentHP >= 10 ? 2 : (currentHP > 0 ? 1 : 0));
        for (int i = 0; i < 3; ++i) {
            m_Stars[i]->SetVisible(i < starsCount);
        }
        LOG_INFO("Victory Menu Visible: {} stars displayed", starsCount);
    }
}

void VictoryMenu::Update() {
    if (!m_Visible) return;

    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
        float cx = m_MainFrame->m_Transform.translation.x;
        float cy = m_MainFrame->m_Transform.translation.y;

        if (IsMouseInsideRect(cx, cy - 65.0f, 180.0f, 50.0f)) {
            m_ContinuePressed = true;
        }
        else if (IsMouseInsideRect(cx, cy - 145.0f, 180.0f, 50.0f)) {
            m_RestartPressed = true;
        }
    }
}

void VictoryMenu::Draw() {
    if (!m_Visible) return;
    // PTSD 會自動繪製 Children
    m_MainFrame->Draw();
}

bool VictoryMenu::IsMouseInsideRect(float x, float y, float w, float h) {
    glm::vec2 mousePos = Util::Input::GetCursorPosition();
    return (mousePos.x >= x - w/2.0f && mousePos.x <= x + w/2.0f &&
            mousePos.y >= y - h/2.0f && mousePos.y <= y + h/2.0f);
}