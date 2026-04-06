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
        SetZIndex(15.0f);
        m_Visible = false;
    }

    void SetVisible(bool visible) { m_Visible = visible; }
    bool IsVisible() const { return m_Visible; }

    // 回傳 Tower::Type 而非 int，避免轉型錯誤
    Tower::Type Update() {
        if (!m_Visible) return Tower::Type::NONE;

        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            return GetSelectedType();
        }
        return Tower::Type::NONE;
    }

    Tower::Type GetSelectedType() {
        glm::vec2 mousePos = Util::Input::GetCursorPosition();
        glm::vec2 offset = mousePos - m_Transform.translation;
        float dist = 45.0f;

        if (glm::distance(offset, glm::vec2{-dist,  dist}) < 30.0f) return Tower::Type::ARCHER;
        if (glm::distance(offset, glm::vec2{ dist,  dist}) < 30.0f) return Tower::Type::BARRACKS;
        if (glm::distance(offset, glm::vec2{-dist, -dist}) < 30.0f) return Tower::Type::MAGE;
        if (glm::distance(offset, glm::vec2{ dist, -dist}) < 30.0f) return Tower::Type::BOMB;

        return Tower::Type::NONE;
    }

    void SetPosition(const glm::vec2& pos) { m_Transform.translation = pos; }

private:
    bool m_Visible = false;
};

#endif