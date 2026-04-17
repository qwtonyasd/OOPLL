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
    enum class Type {
        NONE,
        ARCHER,
        BARRACKS,
        MAGE,
        BOMB
    };
    virtual void UpdateAnimation() {} // 預設不做事
    // 修正：確保這個靜態函式存在於類別中，讓 App.cpp 可以呼叫
    static int GetBaseCost(Type type) {
        switch (type) {
            case Type::ARCHER:   return 70;
            case Type::BARRACKS: return 70;
            case Type::MAGE:     return 100;
            case Type::BOMB:     return 125;
            default:             return 0;
        }
    }

    Tower(const glm::vec2& pos, const std::string& imgPath, float range, float cooldown, float damage, int cost, Enemy::DamageType damageType = Enemy::DamageType::PHYSICAL)
        : m_Range(range), m_Cooldown(cooldown), m_Damage(damage), m_Cost(cost), m_DamageType(damageType) {
        m_Transform.translation = pos;
        SetDrawable(std::make_shared<Util::Image>(imgPath));
        m_ZIndex = 7.0f;
    }

    virtual ~Tower() = default;

    // 修正：如果編譯器抱怨 override，通常是基底類別 GameObject 的 Draw 不是虛擬函式
    // 在這裡我們先拿掉 override 關鍵字，但保留視覺偏移邏輯
    virtual void Draw() {
        if (!m_Drawable) return;

        glm::vec2 originalPos = m_Transform.translation;

        // 套用視覺偏移量
        m_Transform.translation.y += m_VisualOffset;

        // 呼叫父類別的繪製
        GameObject::Draw();

        // 繪製完後立刻還原座標，避免影響邏輯運算
        m_Transform.translation = originalPos;
    }

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
            float dist = glm::distance(m_Transform.translation, enemy->GetPosition());
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
    Enemy::DamageType m_DamageType;

    // 新增：視覺偏移量，預設為 0
    float m_VisualOffset = 0.0f;
};

#endif