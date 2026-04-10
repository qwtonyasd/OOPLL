#ifndef MAGICBOLT_HPP
#define MAGICBOLT_HPP

#include "Projectile.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"
#include "Util/Logger.hpp"
#include <glm/gtc/constants.hpp>

class MagicBolt : public Projectile {
public:
    MagicBolt(glm::vec2 startPos, std::shared_ptr<Enemy> target, float damage)
    : Projectile(startPos, target, damage), m_StartPos(startPos) {

        // 1. 路徑：請確保這裡的路徑與 ArcherTower 一模一樣來測試
        SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/MageTower/Mage/1.png"));

        this->m_IsActive = true;
        this->m_Transform.scale = {1.0f, 1.0f}; // 強制放 5 倍大，大到無法無視
        this->m_Transform.translation = startPos;
        this->m_ZIndex = 1000.0f; // 最高層級

        m_FlightDuration = 0.8f; // 飛慢一點 (1.5秒)，讓你肉眼能捕捉到
        m_MaxArcHeight = 0.0f;   // 關閉拋物線，先走直線排除座標偏移
        m_ElapsedTime = 0.0f;

        m_FinalLandPos = (target) ? target->GetPosition() : startPos;
    }

    // 複寫 Draw 確保它被執行
    void Draw() override {
        if (m_IsActive) {
            GameObject::Draw();
        }
    }

    void Update() override {
        if (!m_IsActive) return;

        float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;
        m_ElapsedTime += dt;

        float progress = std::min(m_ElapsedTime / m_FlightDuration, 1.0f);

        // 紀錄更新前的位置，用來計算向量
        glm::vec2 oldPos = this->m_Transform.translation;

        // --- 計算新位置 ---
        if (m_Target && m_Target->GetHP() > 0) {
            m_FinalLandPos = m_Target->GetPosition();
        }

        glm::vec2 currentGroundPos = glm::mix(m_StartPos, m_FinalLandPos, progress);
        float currentHeight = m_MaxArcHeight * std::sin(progress * glm::pi<float>());

        glm::vec2 newPos = currentGroundPos;
        newPos.y += currentHeight;

        // --- 計算旋轉角度 ---
        glm::vec2 direction = newPos - oldPos;

        // 確保有移動才旋轉，避免 direction 為零時 atan2 出錯
        if (glm::length(direction) > 0.0001f) {
            // atan2 回傳的是弧度 (Radians)
            // 如果你的框架使用弧度：
            this->m_Transform.rotation = std::atan2(direction.y, direction.x);

            // 如果你的框架使用角度 (Degrees)，請換成：
            // this->m_Transform.rotation = std::atan2(direction.y, direction.x) * (180.0f / glm::pi<float>());
        }

        // 更新實際位置
        this->m_Transform.translation = newPos;

        if (progress >= 1.0f) {
            if (m_Target && m_Target->GetHP() > 0) m_Target->TakeDamage(m_Damage);
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