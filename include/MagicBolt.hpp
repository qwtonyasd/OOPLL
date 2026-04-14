#ifndef MAGICBOLT_HPP
#define MAGICBOLT_HPP

#include "Projectile.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"
#include "Util/Logger.hpp"
#include <glm/gtc/constants.hpp>

class MagicBolt : public Projectile {
public:
    MagicBolt(glm::vec2 startPos, std::shared_ptr<Enemy> target, float damage, Enemy::DamageType damageType)
    : Projectile(startPos, target, damage, damageType), m_StartPos(startPos) {

        // 檢查 1：確保圖片路徑正確（請確認資料夾名稱是 MageTower 還是 Mage）
        SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/MageTower/Mage/1.png"));

        this->m_IsActive = true;
        this->m_Transform.scale = {1.0f, 1.0f};
        this->m_Transform.translation = startPos;
        this->m_ZIndex = 100.0f; // 確保 ZIndex 不要大到被背景遮住或小到看不見

        m_FlightDuration = 0.5f; // 縮短飛行時間，讓手感更好
        m_MaxArcHeight = 0.0f;
        m_ElapsedTime = 0.0f;

        m_FinalLandPos = (target) ? target->GetPosition() : startPos;
    }

    // 檢查 2：不要在子類別複寫 Draw，除非你有特殊繪圖需求
    // 如果一定要複寫，請確保有呼叫 GameObject::Draw()
    /*
    void Draw() override {
        if (m_IsActive) {
            GameObject::Draw();
        }
    }
    */

    void Update() override {
        if (!m_IsActive) return;

        float dt = DeltaTime();
        m_ElapsedTime += dt;

        float progress = std::min(m_ElapsedTime / m_FlightDuration, 1.0f);
        glm::vec2 oldPos = this->m_Transform.translation;

        if (m_Target && m_Target->GetHP() > 0) {
            m_FinalLandPos = m_Target->GetPosition();
        }

        glm::vec2 currentGroundPos = glm::mix(m_StartPos, m_FinalLandPos, progress);
        this->m_Transform.translation = currentGroundPos;

        // 計算旋轉
        glm::vec2 direction = currentGroundPos - oldPos;
        if (glm::length(direction) > 0.0001f) {
            this->m_Transform.rotation = std::atan2(direction.y, direction.x);
        }

        if (progress >= 1.0f) {
            if (m_Target && m_Target->GetHP() > 0) {
                m_Target->TakeDamage(m_Damage, m_DamageType);
            }
            m_IsActive = false;
        }
    }

private:
    glm::vec2 m_StartPos;
    glm::vec2 m_FinalLandPos;
    float m_ElapsedTime;
    float m_FlightDuration;
    float m_MaxArcHeight;
};

#endif