#ifndef TOWER_SLOT_HPP
#define TOWER_SLOT_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

// 繼承 Util::GameObject
class TowerSlot : public Util::GameObject {
public:
    TowerSlot(const glm::vec2& pos) {
        m_Transform.translation = pos;
        SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/TowerSlot/1.png"));
        SetZIndex(5.0f); // 讓基座顯示在地圖上方
    }
    void Draw() {
        Util::GameObject::Draw();
    }
};

#endif