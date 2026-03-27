#ifndef TOWER_HPP
#define TOWER_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"
#include "Util/Logger.hpp"
#include "Enemy.hpp"
#include <vector>
#include <memory>

class Tower : public Util::GameObject {
public:
    enum class Type { NONE, ARCHER, MAGE, BARRACKS, BOMB };

    Tower(const glm::vec2& pos, const std::string& imgPath, float range, float cooldown, float damage)
        : m_Range(range), m_Cooldown(cooldown), m_Damage(damage) {
        m_Transform.translation = pos + glm::vec2{0.0f, 10.0f};
        SetDrawable(std::make_shared<Util::Image>(imgPath));
        m_ZIndex = 7.0f;
    }

    virtual ~Tower() = default;

    virtual void Update(std::vector<std::shared_ptr<Enemy>>& enemies) {
        float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;
        m_Timer += dt;

        if (m_Timer >= m_Cooldown) {
            auto target = FindTarget(enemies);
            if (target) {
                Attack(target, enemies);
                m_Timer = 0.0f;
            }
        }
    }

    virtual void Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) = 0;

protected:
    std::shared_ptr<Enemy> FindTarget(const std::vector<std::shared_ptr<Enemy>>& enemies) {
        std::shared_ptr<Enemy> closest = nullptr;
        float minDistance = m_Range;

        for (const auto& enemy : enemies) {
            float dist = glm::distance(m_Transform.translation, enemy->GetTransform().translation);
            if (dist < minDistance) {
                minDistance = dist;
                closest = enemy;
            }
        }
        return closest;
    }

    float m_Range;
    float m_Cooldown;
    float m_Damage;
    float m_Timer = 0.0f;
};

#endif