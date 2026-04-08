#ifndef BUILD_MENU_HPP
#define BUILD_MENU_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Tower.hpp"

class BuildMenu : public Util::GameObject {
public:
    BuildMenu() {
        SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/TowerSlot/5.png"));
        SetZIndex(50.0f);
        m_Visible = false;
    }

    void SetVisible(bool visible) { m_Visible = visible; }
    bool IsVisible() const { return m_Visible; }

    Tower::Type Update() {
        if (!m_Visible) return Tower::Type::NONE;

        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            Tower::Type selected = GetSelectedType();
            if (selected != Tower::Type::NONE) return selected;
        }
        return Tower::Type::NONE;
    }

    Tower::Type GetSelectedType() {
        glm::vec2 mousePos = Util::Input::GetCursorPosition();
        glm::vec2 offset = mousePos - m_Transform.translation;
        float clickRadius = 35.0f;
        float d = 45.0f;

        // 判定邏輯：
        // 左上 (-d, d) -> ARCHER (1)
        if (glm::distance(offset, glm::vec2{-d,  d}) < clickRadius) return Tower::Type::ARCHER;
        // 右上 (d, d)  -> BARRACKS (2)
        if (glm::distance(offset, glm::vec2{ d,  d}) < clickRadius) return Tower::Type::BARRACKS;
        // 左下 (-d, -d)-> MAGE (3)
        if (glm::distance(offset, glm::vec2{-d, -d}) < clickRadius) return Tower::Type::MAGE;
        // 右下 (d, -d) -> BOMB (4)
        if (glm::distance(offset, glm::vec2{ d, -d}) < clickRadius) return Tower::Type::BOMB;

        return Tower::Type::NONE;
    }

    void SetPosition(const glm::vec2& pos) { m_Transform.translation = pos; }

private:
    bool m_Visible = false;
};

#endif