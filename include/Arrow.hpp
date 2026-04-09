#ifndef ARROW_HPP
#define ARROW_HPP

#include "Projectile.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"
#include <glm/gtc/constants.hpp>
#include <algorithm>

class Arrow : public Projectile {
public:
    // 改寫建構子：不需要傳入預判點，只需要起點、目標指標、傷害
    Arrow(glm::vec2 startPos, std::shared_ptr<Enemy> target, float damage)
        : Projectile(startPos, target, damage), m_StartPos(startPos) {
        
        SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/ArcherTower/Arrow/1.png"));
        
        m_ZIndex = 100.0f;
        m_FlightDuration = 0.8f; // 箭矢飛行的總時間（秒）
        m_MaxArcHeight = 50.0f; // 高拋弧度的高度
        m_ElapsedTime = 0.0f;

        // 初始化終點：如果目標存在就設為目標位置，否則設為起點避免錯誤
        if (m_Target) {
            m_FinalLandPos = m_Target->GetPosition();
        } else {
            m_FinalLandPos = startPos;
        }
    }

    void Update() override {
        if (!m_IsActive) return;

        float dt = DeltaTime(); // 使用基類提供的 DeltaTime()
        m_ElapsedTime += dt;
        
        // 1. 【強制追蹤核心】
        // 只要怪物還在且沒死 (HP > 0)，就更新目標終點
        if (m_Target && m_Target->GetHP() > 0) {
            m_FinalLandPos = m_Target->GetPosition();
        }

        // 2. 計算進度 (0.0 到 1.0)
        float progress = std::clamp(m_ElapsedTime / m_FlightDuration, 0.0f, 1.0f);

        // 3. 計算邏輯座標 (地面路徑)
        // 隨 progress 增加，從起點滑向「不斷變動」的終點
        glm::vec2 currentGroundPos = glm::mix(m_StartPos, m_FinalLandPos, progress);

        // 4. 計算高拋偏移 (視覺上的 Y 軸增加)
        float currentHeight = m_MaxArcHeight * std::sin(progress * glm::pi<float>());
        
        // 更新 GameObject 的位移
        m_Transform.translation = currentGroundPos;
        m_Transform.translation.y += currentHeight; 

        // 5. 旋轉邏輯 (讓箭頭指著切線方向)
        UpdateRotation(progress);

        // 6. 落地與命中判定
        if (progress >= 1.0f) {
            // 落地時檢查目標是否還在
            if (m_Target && m_Target->GetHP() > 0) {
                // 檢查落地點跟怪物的距離
                float dist = glm::distance(currentGroundPos, m_Target->GetPosition());
                if (dist < 40.0f) { // 判定半徑
                    m_Target->TakeDamage(m_Damage);
                }
            }
            m_IsActive = false; // 箭矢落地即消失
        }
    }

private:
    void UpdateRotation(float progress) {
        // 取一個微小的未來進度來計算方向向量
        float nextP = std::clamp(progress + 0.01f, 0.0f, 1.0f);
        glm::vec2 nextGroundPos = glm::mix(m_StartPos, m_FinalLandPos, nextP);
        float nextHeight = m_MaxArcHeight * std::sin(nextP * glm::pi<float>());
        
        glm::vec2 nextTotalPos = nextGroundPos;
        nextTotalPos.y += nextHeight;
        
        glm::vec2 dir = nextTotalPos - m_Transform.translation;
        if (glm::length(dir) > 0.0001f) {
            // 計算弧度並存入 Transform
            m_Transform.rotation = std::atan2(dir.y, dir.x);
        }
    }

    glm::vec2 m_StartPos;
    glm::vec2 m_FinalLandPos; // 動態終點
    float m_ElapsedTime;
    float m_FlightDuration;
    float m_MaxArcHeight;
};

#endif