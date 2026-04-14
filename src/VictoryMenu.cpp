#include "VictoryMenu.hpp"
#include "Util/Logger.hpp"

VictoryMenu::VictoryMenu() {
    // 請確認此路徑與你的專案結構一致
    const std::string basePath = "../PTSD/assets/sprites/images/Start/";

    m_Banner = std::make_shared<Util::GameObject>();
    m_Banner->SetDrawable(std::make_shared<Util::Image>(basePath + "12.png"));
    m_Banner->m_Transform.translation = {0, 100};
    m_Banner->SetZIndex(10010.0f); // 設為極高圖層

    m_ChainL = std::make_shared<Util::GameObject>();
    m_ChainL->SetDrawable(std::make_shared<Util::Image>(basePath + "9.png"));
    m_ChainL->m_Transform.translation = {-85, 0};
    m_ChainL->SetZIndex(10000.0f);

    m_ChainR = std::make_shared<Util::GameObject>();
    m_ChainR->SetDrawable(std::make_shared<Util::Image>(basePath + "9.png"));
    m_ChainR->m_Transform.translation = {85, 0};
    m_ChainR->SetZIndex(10000.0f);

    m_BtnContinue = std::make_shared<Util::GameObject>();
    m_BtnContinue->SetDrawable(std::make_shared<Util::Image>(basePath + "11.png"));
    m_BtnContinue->m_Transform.translation = {0, -10};
    m_BtnContinue->SetZIndex(10005.0f);

    m_BtnRestart = std::make_shared<Util::GameObject>();
    m_BtnRestart->SetDrawable(std::make_shared<Util::Image>(basePath + "10.png"));
    m_BtnRestart->m_Transform.translation = {0, -80};
    m_BtnRestart->SetZIndex(10005.0f);

    for (int i = 0; i < 3; ++i) {
        auto star = std::make_shared<Util::GameObject>();
        star->SetDrawable(std::make_shared<Util::Image>(basePath + "13.png"));
        star->m_Transform.translation = {-45.0f + i * 45.0f, 85.0f};
        star->SetZIndex(10015.0f);
        m_Stars.push_back(star);
    }
}

void VictoryMenu::Update() {
    if (!m_Visible) return;

    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
        if (IsMouseOver(m_BtnRestart)) {
            LOG_DEBUG("Restart Pressed");
            m_RestartPressed = true;
        }
        if (IsMouseOver(m_BtnContinue)) {
            LOG_DEBUG("Continue Pressed");
            m_ContinuePressed = true;
        }
    }
}

void VictoryMenu::Draw() {
    if (!m_Visible) return;
    // 依序繪製
    m_ChainL->Draw();
    m_ChainR->Draw();
    m_Banner->Draw();
    m_BtnContinue->Draw();
    m_BtnRestart->Draw();
    for (auto& star : m_Stars) star->Draw();
}

bool VictoryMenu::IsMouseOver(std::shared_ptr<Util::GameObject> obj) {
    glm::vec2 mousePos = Util::Input::GetCursorPosition();
    glm::vec2 objPos = obj->m_Transform.translation;
    // 判定範圍修訂
    return (mousePos.x >= objPos.x - 80.0f && mousePos.x <= objPos.x + 80.0f &&
            mousePos.y >= objPos.y - 30.0f && mousePos.y <= objPos.y + 30.0f);
}