#ifndef VICTORY_MENU_HPP
#define VICTORY_MENU_HPP

#include <vector>
#include <memory>
#include "Util/GameObject.hpp"

class VictoryMenu {
public:
    VictoryMenu();

    void Update();
    void Draw();

    void SetVisible(bool visible, int currentHP = 0);

    // 讓 App 判斷狀態
    bool IsVisible() const { return m_Visible; }

    void ResetFlags() {
        m_ContinuePressed = false;
        m_RestartPressed = false;
    }

    bool IsContinuePressed() const { return m_ContinuePressed; }
    bool IsRestartPressed() const { return m_RestartPressed; }

    // --- 新增：取得根物件，以便掛載到 App 的 m_Root ---
    std::shared_ptr<Util::GameObject> GetGameObject() { return m_MainFrame; }

private:
    bool IsMouseInsideRect(float x, float y, float w, float h);

private:
    std::shared_ptr<Util::GameObject> m_MainFrame; // 18.png
    std::vector<std::shared_ptr<Util::GameObject>> m_Stars; // 13.png

    bool m_Visible = false;
    bool m_ContinuePressed = false;
    bool m_RestartPressed = false;
};

#endif