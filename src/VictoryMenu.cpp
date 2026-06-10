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
        const std::string basePath = "../PTSD/assets/sprites/images/Start/";

        if (currentHP <= 0) {
            m_IsDefeat = true;
            m_MainFrame->SetDrawable(std::make_shared<Util::Image>(basePath + "21.png"));
            for (int i = 0; i < 3; ++i) {
                m_Stars[i]->SetVisible(false);
            }
            LOG_INFO("Defeat Menu Visible: Displaying 21.png");
        } else {
            m_IsDefeat = false;
            m_MainFrame->SetDrawable(std::make_shared<Util::Image>(basePath + "18.png"));

            int starsCount = (currentHP >= 20) ? 3 : (currentHP >= 10 ? 2 : 1);
            for (int i = 0; i < 3; ++i) {
                m_Stars[i]->SetVisible(i < starsCount);
            }
            LOG_INFO("Victory Menu Visible: {} stars displayed (HP: {})", starsCount, currentHP);
        }
    }
}

void VictoryMenu::Update() {
    if (!m_Visible) return;

    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
        glm::vec2 mousePos = Util::Input::GetCursorPosition();
        float cx = m_MainFrame->m_Transform.translation.x;
        float cy = m_MainFrame->m_Transform.translation.y;

        LOG_INFO("=== VictoryMenu Clicked ===");
        LOG_INFO("Mouse Position: ({}, {})", mousePos.x, mousePos.y);

        if (m_IsDefeat) {
            // 💀 💀 💀 戰敗畫面 (21.png) - 根據你測量出的實測數據完美精準判定 💀 💀 💀

            // 1. TRY AGAIN 判定 (實測 X 在 -179 ~ -22，Y 在 -157 ~ -108)
            // 為保證點擊流暢度，邊界皆往外預留 5 像素的容錯空間
            if (mousePos.x >= -184.0f && mousePos.x <= -17.0f &&
                mousePos.y >= -162.0f && mousePos.y <= -103.0f)
            {
                m_RestartPressed = true;
                LOG_INFO("VictoryMenu (Defeat): TRY AGAIN Clicked Successfully!");
            }
            // 2. QUIT 判定 (實測 X 在 15 ~ 172，Y 在 -157 ~ -109)
            else if (mousePos.x >= 10.0f && mousePos.x <= 177.0f &&
                     mousePos.y >= -162.0f && mousePos.y <= -104.0f)
            {
                m_ContinuePressed = true;
                LOG_INFO("VictoryMenu (Defeat): QUIT Clicked Successfully!");
            }
        }
        else {
            // 🏆 🏆 🏆 勝利畫面 (18.png) 判定 - 垂直排列校正區 🏆 🏆 🏆
            if (IsMouseInsideRect(cx, cy - 105.0f, 220.0f, 60.0f)) {
                m_ContinuePressed = true;
                LOG_INFO("VictoryMenu (Victory): CONTINUE Clicked Successfully!");
            }
            else if (IsMouseInsideRect(cx, cy - 185.0f, 220.0f, 60.0f)) {
                m_RestartPressed = true;
                LOG_INFO("VictoryMenu (Victory): RESTART Clicked Successfully!");
            }
        }
    }
}

void VictoryMenu::Draw() {
    if (!m_Visible) return;
    m_MainFrame->Draw();
}

bool VictoryMenu::IsMouseInsideRect(float x, float y, float w, float h) {
    glm::vec2 mousePos = Util::Input::GetCursorPosition();
    return (mousePos.x >= x - w/2.0f && mousePos.x <= x + w/2.0f &&
            mousePos.y >= y - h/2.0f && mousePos.y <= y + h/2.0f);
}