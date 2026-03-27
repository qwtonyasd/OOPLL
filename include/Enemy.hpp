#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Unit.hpp"
#include "Util/Image.hpp"

class Enemy : public Unit {
public:
    Enemy(const std::vector<glm::vec2>& path, float speed, float hp, const std::string& imgPath)
        : Unit(path, speed, hp), m_IsBlocked(false) {
        SetDrawable(std::make_shared<Util::Image>(imgPath));
        SetZIndex(10.0f);
    }

    void Update() override {
        if (m_HP <= 0) return;

        // 如果沒有被小兵擋住，才繼續走
        if (!m_IsBlocked) {
            if (m_CurrentNodeIdx < m_Path.size()) {
                MoveTowardsNextNode();
            } else {
                m_HP = 0;
            }
        }
    }

    void SetBlocked(bool blocked) { m_IsBlocked = blocked; }
    bool IsBlocked() const { return m_IsBlocked; }
    void TakeDamage(float amount) { m_HP -= amount; }

private:
    bool m_IsBlocked;
};

#endif