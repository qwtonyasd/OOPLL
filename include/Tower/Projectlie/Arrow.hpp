#ifndef ARROW_HPP
#define ARROW_HPP

#include "Tower/Projectlie/Projectile.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"
#include <glm/gtc/constants.hpp>
#include <algorithm>
#include <vector>
#include <string>

class Arrow : public Projectile {
public:
    Arrow(glm::vec2 startPos, std::shared_ptr<Enemy> target, float damage, Enemy::DamageType damageType)
        : Projectile(startPos, target, damage, damageType), m_StartPos(startPos) {

        SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/ArcherTower/Arrow/1.png"));

        // 預先準備好特效路徑（這裡你可以根據實際張數調整）
        std::string effectPath = "../PTSD/assets/sprites/images/ArcherTower/Arrow/";
        for (int i = 2; i <= 11; ++i) {
            m_HitFrames.push_back(effectPath + std::to_string(i) + ".png");
        }

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

        if (m_IsHitting) {
            // --- 命中特效階段 ---
            m_HitTimer += dt;
            int frameIndex = static_cast<int>(m_HitTimer / 0.05f); // 0.05秒換一幀

            if (frameIndex < static_cast<int>(m_HitFrames.size())) {
                SetDrawable(std::make_shared<Util::Image>(m_HitFrames[frameIndex]));
            } else {
                m_IsActive = false; // 特效播完，箭矢才真正消失
            }
        } else {
            // --- 飛行階段 ---
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
                        m_Target->TakeDamage(m_Damage, m_DamageType);
                    }
                }
                // 進入命中狀態，不直接設為 inactive
                m_IsHitting = true;
                m_Transform.rotation = 0.0f; // 命中時通常不旋轉
            }
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

    // 新增狀態控制
    bool m_IsHitting = false;
    float m_HitTimer = 0.0f;
    std::vector<std::string> m_HitFrames;

    glm::vec2 m_StartPos;
    glm::vec2 m_FinalLandPos;
    float m_ElapsedTime;
    float m_FlightDuration;
    float m_MaxArcHeight;
};

#endif