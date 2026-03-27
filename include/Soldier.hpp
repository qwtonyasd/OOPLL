#ifndef SOLDIER_HPP
#define SOLDIER_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"

class Soldier : public Util::GameObject {
public:
    Soldier(glm::vec2 pos) : m_HomePos(pos), m_HP(50.0f) {
        SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/BarracksTower/Soldier/1.png"));
        m_Transform.translation = pos;
        m_ZIndex = 11.0f;
    }

    // 必須真的寫出來，不能只有註解
    void Update(float dt, bool inCombat) {
        if (IsDead()) {
            SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/BarracksTower/Soldier/24.png"));
            return;
        }
        if (!inCombat && m_HP < 50.0f) {
            m_HP = std::min(50.0f, m_HP + 5.0f * dt);
        }
    }

    void Respawn() {
        m_HP = 50.0f;
        SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/BarracksTower/Soldier/1.png"));
    }

    bool IsDead() const { return m_HP <= 0; }
    void TakeDamage(float amount) { m_HP -= amount; }

private:
    glm::vec2 m_HomePos;
    float m_HP;
};

#endif