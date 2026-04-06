#ifndef TOWER_SLOT_HPP
#define TOWER_SLOT_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

class TowerSlot : public Util::GameObject {
public:
    explicit TowerSlot(const glm::vec2& pos) {
        m_Transform.translation = pos;
        // 修正路徑：確保跟 BuildMenu 一樣指向 PTSD 資料夾
        SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/TowerSlot/1.png"));
        SetZIndex(5.0f);
    }

    glm::vec2 GetPosition() const { return m_Transform.translation; }
    bool IsOccupied() const { return m_IsOccupied; }
    void SetOccupied(bool occupied) { m_IsOccupied = occupied; }

private:
    bool m_IsOccupied = false;
};

#endif