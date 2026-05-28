#pragma once
#include <glm/vec2.hpp>
#include <string>
#include <memory>

class Spell {
protected:
    float m_Cooldown = 0.0f;       // 總冷卻時間
    float m_CooldownTimer = 0.0f;  // 當前冷卻計時器
    bool m_IsReady = true;         // 是否可以使用
    std::string m_IconPath;        // 技能圖示路徑

public:
    Spell(float cooldown, std::string iconPath) 
        : m_Cooldown(cooldown), m_IconPath(iconPath) {}
    virtual ~Spell() = default;

    virtual void Update(float dt) {
        if (!m_IsReady) {
            m_CooldownTimer += dt;
            if (m_CooldownTimer >= m_Cooldown) {
                m_IsReady = true;
                m_CooldownTimer = 0.0f;
            }
        }
    }

    // 當玩家點擊地圖某個座標時觸發法術
    virtual void Cast(glm::vec2 clickPos) = 0;

    bool IsReady() const { return m_IsReady; }
    float GetCooldownPercent() const { return m_CooldownTimer / m_Cooldown; } // 用於 UI 轉圈圈
    void StartCooldown() { m_IsReady = false; m_CooldownTimer = 0.0f; }
};