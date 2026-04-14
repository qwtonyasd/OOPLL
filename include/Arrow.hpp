#ifndef ARROW_HPP
#define ARROW_HPP

#include "Projectile.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"
#include <glm/gtc/constants.hpp>
#include <algorithm>

class Arrow : public Projectile {
public:
    // 修正點：參數新增 Enemy::DamageType
    Arrow(glm::vec2 startPos, std::shared_ptr<Enemy> target, float damage, Enemy::DamageType damageType)
        : Projectile(startPos, target, damage, damageType), m_StartPos(startPos) {

        SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/ArcherTower/Arrow/1.png"));

        m_ZIndex = 100.0f;
        m_FlightDuration = 0.8f;
        m_MaxArcHeight = 50.0f;
        m_ElapsedTime = 0.0f;

        if (m_Target) {
            m_FinalLandPos = m_Target->GetPosition();
        } else {
            m_FinalLandPos = startPos;
        }
    }

    void Update() override {
        if (!m_IsActive) return;

        float dt = DeltaTime();
        m_ElapsedTime += dt;

        if (m_Target && m_Target->GetHP() > 0) {
            m_FinalLandPos = m_Target->GetPosition();
        }

        float progress = std::clamp(m_ElapsedTime / m_FlightDuration, 0.0f, 1.0f);
        glm::vec2 currentGroundPos = glm::mix(m_StartPos, m_FinalLandPos, progress);
        float currentHeight = m_MaxArcHeight * std::sin(progress * glm::pi<float>());

        m_Transform.translation = currentGroundPos;
        m_Transform.translation.y += currentHeight;

        UpdateRotation(progress);

        if (progress >= 1.0f) {
            if (m_Target && m_Target->GetHP() > 0) {
                float dist = glm::distance(currentGroundPos, m_Target->GetPosition());
                if (dist < 40.0f) {
                    // 使用帶有 DamageType 的 TakeDamage
                    m_Target->TakeDamage(m_Damage, m_DamageType);
                }
            }
            m_IsActive = false;
        }
    }

private:
    void UpdateRotation(float progress) {
        float nextP = std::clamp(progress + 0.01f, 0.0f, 1.0f);
        glm::vec2 nextGroundPos = glm::mix(m_StartPos, m_FinalLandPos, nextP);
        float nextHeight = m_MaxArcHeight * std::sin(nextP * glm::pi<float>());

        glm::vec2 nextTotalPos = nextGroundPos;
        nextTotalPos.y += nextHeight;

        glm::vec2 dir = nextTotalPos - m_Transform.translation;
        if (glm::length(dir) > 0.0001f) {
            m_Transform.rotation = std::atan2(dir.y, dir.x);
        }
    }

    glm::vec2 m_StartPos;
    glm::vec2 m_FinalLandPos;
    float m_ElapsedTime;
    float m_FlightDuration;
    float m_MaxArcHeight;
};

#endif