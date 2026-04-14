#ifndef VICTORY_MENU_HPP
#define VICTORY_MENU_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
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

    std::shared_ptr<Util::GameObject> m_Banner;   // 12.png
    std::shared_ptr<Util::GameObject> m_ChainL;   // 9.png
    std::shared_ptr<Util::GameObject> m_ChainR;   // 9.png
    std::shared_ptr<Util::GameObject> m_BtnRestart;  // 10.png
    std::shared_ptr<Util::GameObject> m_BtnContinue; // 11.png
    std::vector<std::shared_ptr<Util::GameObject>> m_Stars; // 13.png

    bool IsMouseOver(std::shared_ptr<Util::GameObject> obj);
};

#endif