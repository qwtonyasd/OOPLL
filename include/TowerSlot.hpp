#ifndef TOWER_SLOT_HPP
#define TOWER_SLOT_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

class TowerSlot : public Util::GameObject {
public:
    TowerSlot(const glm::vec2& pos) {
        m_Transform.translation = pos;
        SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/TowerSlot/1.png"));
        SetZIndex(5.0f);
    }

    // --- 新增：記錄狀態 ---
    bool IsOccupied() const { return m_IsOccupied; }
    void SetOccupied(bool occupied) { m_IsOccupied = occupied; }

private:
    bool m_IsOccupied = false;
};

#endif