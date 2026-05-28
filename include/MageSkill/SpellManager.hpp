#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <iostream>
#include "ReinforcementSpell.hpp" // 確保正確引入
#include "Enemy.hpp"
#include "Soldier.hpp"

class SpellManager {
public:
    enum class SpellType { NONE, FIREBALL, REINFORCE };

    SpellManager() = default;

    // --- 恢復這些必要的介面以符合 App.cpp 與 Hud.cpp 的呼叫 ---
    void Update(float dt) {
        if (m_FireballCooldownTimer > 0.0f) m_FireballCooldownTimer -= dt;
        if (m_ReinforceCooldownTimer > 0.0f) m_ReinforceCooldownTimer -= dt;
    }

    void SelectFireball() { if (m_FireballCooldownTimer <= 0.0f) m_CurrentSelectedSpell = SpellType::FIREBALL; }
    void SelectReinforce() { if (m_ReinforceCooldownTimer <= 0.0f) m_CurrentSelectedSpell = SpellType::REINFORCE; }
    void CancelSelection() { m_CurrentSelectedSpell = SpellType::NONE; }

    // 用於 UI 的判斷
    SpellType GetSelectedSpell() const { return m_CurrentSelectedSpell; }
    float GetFireballCooldownRatio() const { return m_FireballCooldownTimer / 15.0f; }
    float GetReinforceCooldownRatio() const { return m_ReinforceCooldownTimer / 10.0f; }

    void CastCurrentSpell(const glm::vec2& clickPos,
                          std::vector<std::shared_ptr<Enemy>>& enemies,
                          std::vector<std::shared_ptr<Soldier>>& stageSoldiers) {
        if (m_CurrentSelectedSpell == SpellType::NONE) return;

        if (m_CurrentSelectedSpell == SpellType::REINFORCE) {
            // 這裡委派給 ReinforcementSpell 執行
            ReinforcementSpell rs(stageSoldiers);
            rs.Cast(clickPos);
            m_ReinforceCooldownTimer = 10.0f;
        }
        else if (m_CurrentSelectedSpell == SpellType::FIREBALL) {
            // 火球邏輯 (可選)
            m_FireballCooldownTimer = 15.0f;
        }

        m_CurrentSelectedSpell = SpellType::NONE;
    }

private:
    SpellType m_CurrentSelectedSpell = SpellType::NONE;
    float m_FireballCooldownTimer = 0.0f;
    float m_ReinforceCooldownTimer = 0.0f;
};