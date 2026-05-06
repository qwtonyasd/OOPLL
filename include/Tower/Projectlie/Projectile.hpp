#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "Util/GameObject.hpp"
#include "Util/Time.hpp"
#include "Enemy.hpp"
#include <memory>
#include <glm/glm.hpp>
#include "Util/Logger.hpp"

class Projectile : public Util::GameObject {
public:
    // 確保這裡有 4 個參數
    Projectile(glm::vec2 startPos, std::shared_ptr<Enemy> target, float damage, Enemy::DamageType damageType)
        : m_Target(target), m_Damage(damage), m_DamageType(damageType) {
        m_Transform.translation = startPos;
        m_ZIndex = 50.0f;
    }

    virtual ~Projectile() = default;
    virtual void Update() = 0;
    bool IsActive() const { return m_IsActive; }

    // 移除 override，因為 GameObject::Draw 可能是最上層
    virtual void Draw() {
        if (!m_IsActive) return;
        GameObject::Draw();
    }

protected:
    std::shared_ptr<Enemy> m_Target;
    float m_Damage;
    Enemy::DamageType m_DamageType;
    float m_Speed = 600.0f;
    bool m_IsActive = true;

    float DeltaTime() const {
        return static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;
    }
};

#endif