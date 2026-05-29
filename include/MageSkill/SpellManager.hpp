#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "ReinforcementSpell.hpp"
#include "FireballSpell.hpp"
#include "FireballProjectile.hpp"
#include "Enemy.hpp"
#include "Soldier.hpp"

class SpellManager {
public:
    enum class SpellType { NONE, FIREBALL, REINFORCE };

    SpellManager() = default;

    void Update(float dt, std::vector<std::shared_ptr<Enemy>>& enemies) {
        if (m_FireballCooldownTimer > 0.0f) m_FireballCooldownTimer -= dt;
        if (m_ReinforceCooldownTimer > 0.0f) m_ReinforceCooldownTimer -= dt;

        // 更新所有飛行中的火球
        for (auto it = m_ActiveFireballs.begin(); it != m_ActiveFireballs.end(); ) {
            if (!(*it)->Update(enemies, dt)) {
                it = m_ActiveFireballs.erase(it);
            } else {
                ++it;
            }
        }
    }

    void Draw() {
        for (auto& f : m_ActiveFireballs) f->Draw();
    }

    void SelectFireball() { if (m_FireballCooldownTimer <= 0.0f) m_CurrentSelectedSpell = SpellType::FIREBALL; }
    void SelectReinforce() { if (m_ReinforceCooldownTimer <= 0.0f) m_CurrentSelectedSpell = SpellType::REINFORCE; }
    void CancelSelection() { m_CurrentSelectedSpell = SpellType::NONE; }

    SpellType GetSelectedSpell() const { return m_CurrentSelectedSpell; }
    float GetFireballCooldownRatio() const { return m_FireballCooldownTimer / 15.0f; }
    float GetReinforceCooldownRatio() const { return m_ReinforceCooldownTimer / 10.0f; }

    void CastCurrentSpell(const glm::vec2& clickPos,
                          std::vector<std::shared_ptr<Enemy>>& enemies,
                          std::vector<std::shared_ptr<Soldier>>& stageSoldiers) {
        if (m_CurrentSelectedSpell == SpellType::NONE) return;

        if (m_CurrentSelectedSpell == SpellType::REINFORCE) {
            ReinforcementSpell rs(stageSoldiers);
            rs.Cast(clickPos);
            m_ReinforceCooldownTimer = 10.0f;
        }
        else if (m_CurrentSelectedSpell == SpellType::FIREBALL) {
            // 從螢幕頂部落
            glm::vec2 startPos = {clickPos.x, 800.0f};
            m_ActiveFireballs.push_back(std::make_shared<FireballProjectile>(startPos, clickPos, 100.0f, 80.0f));
            m_FireballCooldownTimer = 15.0f;
        }

        m_CurrentSelectedSpell = SpellType::NONE;
    }

private:
    SpellType m_CurrentSelectedSpell = SpellType::NONE;
    float m_FireballCooldownTimer = 0.0f;
    float m_ReinforceCooldownTimer = 0.0f;
    std::vector<std::shared_ptr<FireballProjectile>> m_ActiveFireballs;
};