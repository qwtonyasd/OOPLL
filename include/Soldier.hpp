#ifndef SOLDIER_HPP
#define SOLDIER_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Enemy.hpp"
#include <algorithm>

class Soldier : public Util::GameObject {
public:
    Soldier(glm::vec2 pos) : m_HomePos(pos), m_HP(50.0f) {
        SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/BarracksTower/Soldier/1.png"));
        m_Transform.translation = pos;
        m_ZIndex = 11.0f;
    }

    // 範圍定義：數值越大，攔截怪物的能力越強
    float GetBlockRange() const { return 45.0f; }   // 必須在此距離內才能「擋住」怪
    float GetSupportRange() const { return 80.0f; } // 只要在此距離內，就算怪被隊友擋住也能幫打

    void Update(float dt, std::shared_ptr<Enemy> target) {
        if (IsDead()) {
            SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/BarracksTower/Soldier/24.png"));
            return;
        }

        if (target) {
            // 戰鬥中：每秒對怪造成傷害
            target->TakeDamage(15.0f * dt);
        } else {
            // 非戰鬥：緩慢回血
            if (m_HP < 50.0f) m_HP = std::min(50.0f, m_HP + 5.0f * dt);
            SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/BarracksTower/Soldier/1.png"));
        }
    }

    void TakeDamage(float amount) { m_HP -= amount; }
    bool IsDead() const { return m_HP <= 0; }
    void Respawn() {
        m_HP = 50.0f;
        SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/BarracksTower/Soldier/1.png"));
    }

private:
    glm::vec2 m_HomePos;
    float m_HP;
};

#endif