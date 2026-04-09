#ifndef TOWER_HPP
#define TOWER_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"
#include "Enemy.hpp"
#include "Projectile.hpp"
#include <vector>
#include <memory>
#include <glm/glm.hpp>

class Tower : public Util::GameObject {
public:

    virtual void Draw() {
        GameObject::Draw(); // 呼叫父類別原本的繪製邏輯
    }
    enum class Type {
        NONE,
        ARCHER,
        BARRACKS,
        MAGE,
        BOMB
    };

    static int GetBaseCost(Type type) {
        switch (type) {
            case Type::ARCHER:   return 70;
            case Type::BARRACKS: return 70;
            case Type::MAGE:     return 100;
            case Type::BOMB:     return 125;
            default:             return 0;
        }
    }

    Tower(const glm::vec2& pos, const std::string& imgPath, float range, float cooldown, float damage, int cost)
        : m_Range(range), m_Cooldown(cooldown), m_Damage(damage), m_Cost(cost) {
        m_Transform.translation = pos;
        SetDrawable(std::make_shared<Util::Image>(imgPath));
        m_ZIndex = 7.0f;
    }

    virtual ~Tower() = default;

    virtual void Update(std::vector<std::shared_ptr<Enemy>>& enemies,
                        std::vector<std::shared_ptr<Projectile>>& projectiles) {
        float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;
        m_Timer += dt;

        if (m_Timer >= m_Cooldown) {
            auto target = FindTarget(enemies);
            if (target) {
                Attack(target, enemies, projectiles);
                m_Timer = 0.0f;
            }
        }
    }

    virtual void Attack(std::shared_ptr<Enemy> target,
                        std::vector<std::shared_ptr<Enemy>>& allEnemies,
                        std::vector<std::shared_ptr<Projectile>>& projectiles) = 0;

protected:
    std::shared_ptr<Enemy> FindTarget(const std::vector<std::shared_ptr<Enemy>>& enemies) {
        std::shared_ptr<Enemy> closest = nullptr;
        float minDistance = m_Range;
        for (const auto& enemy : enemies) {
            if (!enemy || enemy->GetHP() <= 0) continue;
            float dist = glm::distance(m_Transform.translation, enemy->GetTransform().translation);
            if (dist < minDistance) {
                minDistance = dist;
                closest = enemy;
            }
        }
        return closest;
    }

    float m_Range, m_Cooldown, m_Damage;
    float m_Timer = 0.0f;
    int m_Cost;
};

#endif