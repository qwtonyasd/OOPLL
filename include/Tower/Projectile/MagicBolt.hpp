#ifndef MAGICBOLT_HPP
#define MAGICBOLT_HPP

#include "Tower/Projectile/Projectile.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"
#include "Util/Logger.hpp"
#include <glm/gtc/constants.hpp>
#include <vector>
#include <string>

class MagicBolt : public Projectile {
public:
    MagicBolt(glm::vec2 startPos, std::shared_ptr<Enemy> target, float damage, Enemy::DamageType damageType)
    : Projectile(startPos, target, damage, damageType), m_StartPos(startPos) {

        // 初始圖片
        SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/MageTower/Mage/1.png"));

        // 1. 自動載入命中特效路徑 (假設有 6 張圖)
        std::string effectPath = "../PTSD/assets/sprites/images/MageTower/Mage/";
        for (int i = 2; i <= 9; ++i) {
            m_HitFrames.push_back(effectPath + std::to_string(i) + ".png");
        }

        this->m_IsActive = true;
        this->m_Transform.scale = {1.0f, 1.0f};
        this->m_Transform.translation = startPos;
        this->m_ZIndex = 100.0f;

        m_FlightDuration = 0.4f; // 法師球通常飛得比箭矢快
        m_ElapsedTime = 0.0f;

        m_FinalLandPos = (target) ? target->GetPosition() : startPos;
    }

    void Update() override {
        if (!m_IsActive) return;

        float dt = DeltaTime();

        if (m_IsHitting) {
            // --- 命中特效階段 ---
            m_HitTimer += dt;
            int frameIndex = static_cast<int>(m_HitTimer / 0.05f);

            if (frameIndex < static_cast<int>(m_HitFrames.size())) {
                SetDrawable(std::make_shared<Util::Image>(m_HitFrames[frameIndex]));
            } else {
                m_IsActive = false; // 動畫播完消失
            }
        } else {
            // --- 飛行階段 ---
            m_ElapsedTime += dt;
            float progress = std::min(m_ElapsedTime / m_FlightDuration, 1.0f);

            if (m_Target && m_Target->GetHP() > 0) {
                m_FinalLandPos = m_Target->GetPosition();
            }

            // 直線移動
            glm::vec2 oldPos = this->m_Transform.translation;
            glm::vec2 currentPos = glm::mix(m_StartPos, m_FinalLandPos, progress);
            this->m_Transform.translation = currentPos;

            // 計算旋轉（讓法師球尖端朝向敵人）
            glm::vec2 direction = currentPos - oldPos;
            if (glm::length(direction) > 0.0001f) {
                this->m_Transform.rotation = std::atan2(direction.y, direction.x);
            }

            if (progress >= 1.0f) {
                if (m_Target && m_Target->GetHP() > 0) {
                    m_Target->TakeDamage(m_Damage, m_DamageType);
                }
                // 切換到命中狀態
                m_IsHitting = true;
                this->m_Transform.rotation = 0.0f; // 特效通常不旋轉
            }
        }
    }

private:
    // 狀態與動畫變數
    bool m_IsHitting = false;
    float m_HitTimer = 0.0f;
    std::vector<std::string> m_HitFrames;

    glm::vec2 m_StartPos;
    glm::vec2 m_FinalLandPos;
    float m_ElapsedTime;
    float m_FlightDuration;
};

#endif