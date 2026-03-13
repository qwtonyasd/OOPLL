#ifndef TOWER_SLOT_HPP
#define TOWER_SLOT_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

// 繼承 Util::GameObject
class TowerSlot : public Util::GameObject {
public:
    TowerSlot(const glm::vec2& pos) {
        m_Transform.translation = pos;
        // 這裡設定圖片與 ZIndex
        SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/1.png"));
        SetZIndex(5.0f); // 讓基座顯示在地圖上方
    }

    // 關鍵修正：不要覆寫 Draw()，直接用父類別的 GameObject::Draw()
    // 或者如果你一定要寫，就呼叫 GameObject::Draw()
    void Draw() {
        Util::GameObject::Draw();
    }
};

#endif