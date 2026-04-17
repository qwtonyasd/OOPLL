#ifndef VICTORY_MENU_HPP
#define VICTORY_MENU_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include <vector>
#include <memory>

class VictoryMenu {
public:
    VictoryMenu();
    void Update();
    void Draw();
    void SetVisible(bool visible) { m_Visible = visible; }
    bool IsVisible() const { return m_Visible; }

    bool IsRestartPressed() const { return m_RestartPressed; }
    bool IsContinuePressed() const { return m_ContinuePressed; }
    void ResetFlags() { m_RestartPressed = false; m_ContinuePressed = false; }

private:
    bool m_Visible = false;
    bool m_RestartPressed = false;
    bool m_ContinuePressed = false;

    std::shared_ptr<Util::GameObject> m_MainFrame; // 合成圖 18.png
    std::vector<std::shared_ptr<Util::GameObject>> m_Stars; // 星星 13.png

    // 輔助函式：檢查點擊範圍
    bool IsMouseInsideRect(float centerX, float centerY, float width, float height);
};

#endif