#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Unit.hpp"
#include "Util/Image.hpp"

class Enemy : public Unit {
public:
    Enemy(const std::vector<glm::vec2>& path, float speed, float hp, const std::string& imgPath)
        : Unit(path, speed, hp) {
        SetDrawable(std::make_shared<Util::Image>(imgPath));
        SetZIndex(10.0f); // 怪物在地圖上方
    }

    void Update() override {
        if (m_CurrentNodeIdx < m_Path.size()) {
            MoveTowardsNextNode();
        } else {
            m_HP = 0; // 抵達終點，將自己標記為死亡以便 App 清除
        }
    }
};

#endif