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
    Projectile(glm::vec2 startPos, std::shared_ptr<Enemy> target, float damage)
        : m_Target(target), m_Damage(damage) {
        m_Transform.translation = startPos;
    }

    virtual ~Projectile() = default;

    virtual void Update() = 0;

    bool IsActive() const { return m_IsActive; }

    // --- 這裡修正：移除 override ---
    virtual void Draw() {
        if (!m_IsActive) return;

        // 1. 執行原本的繪製
        GameObject::Draw();

        // 2. 暴力除錯：在 Console 印出目前子彈的詳細資訊
        // 如果這個 Log 有跳，代表子彈邏輯是活著的
        LOG_DEBUG("Projectile Type: {} | Pos: ({}, {}) | Z: {}",
                  typeid(*this).name(),
                  m_Transform.translation.x,
                  m_Transform.translation.y,
                  m_ZIndex);
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