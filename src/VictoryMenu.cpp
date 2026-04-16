#include "VictoryMenu.hpp"
#include "Util/Logger.hpp"

VictoryMenu::VictoryMenu() {
    const std::string basePath = "../PTSD/assets/sprites/images/Start/";

    // 1. Banner
    m_Banner = std::make_shared<Util::GameObject>();
    m_Banner->SetDrawable(std::make_shared<Util::Image>(basePath + "12.png"));
    m_Banner->m_Transform.translation = {0.0f, 100.0f};
    m_Banner->m_Transform.scale = {1.0f, 1.0f};
    m_Banner->SetZIndex(100.0f); // 只要大於地圖的 0 即可

    // 2. Restart Button
    m_BtnRestart = std::make_shared<Util::GameObject>();
    m_BtnRestart->SetDrawable(std::make_shared<Util::Image>(basePath + "10.png"));
    m_BtnRestart->m_Transform.translation = {0.0f, -80.0f};
    m_BtnRestart->m_Transform.scale = {1.0f, 1.0f};
    m_BtnRestart->SetZIndex(101.0f);

    // 3. Continue Button
    m_BtnContinue = std::make_shared<Util::GameObject>();
    m_BtnContinue->SetDrawable(std::make_shared<Util::Image>(basePath + "11.png"));
    m_BtnContinue->m_Transform.translation = {0.0f, -10.0f};
    m_BtnContinue->m_Transform.scale = {1.0f, 1.0f};
    m_BtnContinue->SetZIndex(101.0f);

    // 4. Stars
    for (int i = 0; i < 3; ++i) {
        auto star = std::make_shared<Util::GameObject>();
        star->SetDrawable(std::make_shared<Util::Image>(basePath + "13.png"));
        star->m_Transform.translation = {-45.0f + i * 45.0f, 85.0f};
        star->m_Transform.scale = {1.0f, 1.0f};
        star->SetZIndex(102.0f);
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

    // 強制按順序繪製子物件
    m_Banner->Draw();
    m_BtnContinue->Draw();
    m_BtnRestart->Draw();
    for (auto& star : m_Stars) star->Draw();
}

bool VictoryMenu::IsMouseOver(std::shared_ptr<Util::GameObject> obj) {
    glm::vec2 mousePos = Util::Input::GetCursorPosition();
    glm::vec2 objPos = obj->m_Transform.translation;
    // 判定框範圍加寬，增加點擊容錯率
    return (mousePos.x >= objPos.x - 100.0f && mousePos.x <= objPos.x + 100.0f &&
            mousePos.y >= objPos.y - 40.0f && mousePos.y <= objPos.y + 40.0f);
}