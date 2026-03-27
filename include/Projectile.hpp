#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "Util/GameObject.hpp"
#include "Util/Time.hpp"
#include "Enemy.hpp"
#include <memory>

class Projectile : public Util::GameObject {
public:
    Projectile(glm::vec2 startPos, std::shared_ptr<Enemy> target, float damage)
        : m_Target(target), m_Damage(damage) {
        m_Transform.translation = startPos;
    }

    virtual ~Projectile() = default;

    // 純虛擬函式：強迫子類別實作自己的飛行與命中邏輯
    virtual void Update() = 0;
    virtual void OnHit() = 0;

    bool IsActive() const { return m_IsActive; }

protected:
    std::shared_ptr<Enemy> m_Target;
    float m_Damage;
    float m_Speed = 600.0f; // 預設速度
    bool m_IsActive = true;

    // 輔助函式：取得 DeltaTime (秒)
    float DeltaTime() const {
        return static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;
    }
};

#endif