#ifndef BOMB_HPP
#define BOMB_HPP

#include "Projectile.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"
#include "Util/Logger.hpp"
#include <glm/gtc/constants.hpp>
#include <vector>
#include <algorithm>

class Bomb : public Projectile {
public:
    // 修正點：建構子參數匹配 Projectile
    Bomb(glm::vec2 startPos, glm::vec2 targetPos, float damage, std::vector<std::shared_ptr<Enemy>>& enemies, Enemy::DamageType damageType)
        : Projectile(startPos, nullptr, damage, damageType),
          m_StartPos(startPos),
          m_FinalLandPos(targetPos),
          m_Enemies(enemies) {

        SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/BombTower/bomb/1.png"));

        m_IsActive = true;
        m_Transform.scale = {1.0f, 1.0f};
        m_Transform.translation = startPos;
        m_ZIndex = 50.0f;

        m_FlightDuration = 1.0f;
        m_MaxArcHeight = 100.0f;
        m_ElapsedTime = 0.0f;
    }

    void Update() override {
        if (!m_IsActive) return;

        float dt = DeltaTime();
        m_ElapsedTime += dt;

        float progress = std::clamp(m_ElapsedTime / m_FlightDuration, 0.0f, 1.0f);
        glm::vec2 currentPos = glm::mix(m_StartPos, m_FinalLandPos, progress);
        float height = m_MaxArcHeight * std::sin(progress * glm::pi<float>());

        m_Transform.translation = currentPos;
        m_Transform.translation.y += height;

        if (progress >= 1.0f) {
            Explode();
            m_IsActive = false;
        }
    }

private:
    void Explode() {
        float radius = 80.0f;
        for (auto& enemy : m_Enemies) {
            if (enemy && enemy->GetHP() > 0) {
                if (glm::distance(m_FinalLandPos, enemy->GetPosition()) <= radius) {
                    // 傳入砲彈的 PHYSICAL 傷害類型
                    enemy->TakeDamage(m_Damage, m_DamageType);
                }
            }
        }
    }

    glm::vec2 m_StartPos;
    glm::vec2 m_FinalLandPos;
    std::vector<std::shared_ptr<Enemy>>& m_Enemies;
    float m_ElapsedTime;
    float m_FlightDuration;
    float m_MaxArcHeight;
};

#endif