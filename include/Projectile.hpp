#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "Util/GameObject.hpp"
#include "Util/Time.hpp"
#include "Enemy.hpp"
#include <memory>
#include <glm/glm.hpp>

class Projectile : public Util::GameObject {
public:
    Projectile(glm::vec2 startPos, std::shared_ptr<Enemy> target, float damage)
        : m_Target(target), m_Damage(damage) {
        m_Transform.translation = startPos;
    }

    virtual ~Projectile() = default;

    virtual void Update() = 0; // 每個子彈移動方式不同

    bool IsActive() const { return m_IsActive; }

    virtual void Draw() {
        if (m_IsActive) {
            // LOG_DEBUG("Drawing Projectile at {}, {}", m_Transform.translation.x, m_Transform.translation.y);
            GameObject::Draw();
        }
    }
protected:
    std::shared_ptr<Enemy> m_Target;
    float m_Damage;
    float m_Speed = 600.0f;
    bool m_IsActive = true;

    float DeltaTime() const {
        return static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;
    }
};


#endif