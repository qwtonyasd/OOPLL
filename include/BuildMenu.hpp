#ifndef BUILD_MENU_HPP
#define BUILD_MENU_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Tower.hpp"

class BuildMenu : public Util::GameObject {
public:
    BuildMenu() {
        SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/TowerSlot/5.png"));
        SetZIndex(15.0f);
        m_Visible = false;
    }

    void Show(const glm::vec2& pos) {
        m_Transform.translation = pos;
        m_Visible = true;
    }

    void Hide() { m_Visible = false; }
    bool IsVisible() const { return m_Visible; }

    // --- 核心修正：範圍偵測 ---
    Tower::Type GetSelectedType() {
        glm::vec2 mousePos = Util::Input::GetCursorPosition();
        // 計算滑鼠相對於選單中心的偏移
        glm::vec2 offset = mousePos - m_Transform.translation;

        // 定義四個按鈕的中心座標 (根據 5.png 調整，以下為推測數值)
        float distFromCenter = 35.0f; // 圖標中心距離選單中心的距離
        float clickRadius = 20.0f;    // 每個小圖標的可點擊半徑

        glm::vec2 archerPos   = {-distFromCenter,  distFromCenter}; // 左上
        glm::vec2 barracksPos = { distFromCenter,  distFromCenter}; // 右上
        glm::vec2 magePos     = {-distFromCenter, -distFromCenter}; // 左下
        glm::vec2 bombPos     = { distFromCenter, -distFromCenter}; // 右下

        if (glm::distance(offset, archerPos)   < clickRadius) return Tower::Type::ARCHER;
        if (glm::distance(offset, barracksPos) < clickRadius) return Tower::Type::BARRACKS;
        if (glm::distance(offset, magePos)     < clickRadius) return Tower::Type::MAGE;
        if (glm::distance(offset, bombPos)     < clickRadius) return Tower::Type::BOMB;

        return Tower::Type::NONE; // 如果點在空隙處，回傳 NONE
    }

private:
    bool m_Visible = false;
};

#endif