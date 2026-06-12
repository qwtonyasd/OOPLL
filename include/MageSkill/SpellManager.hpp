#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "ReinforcementSpell.hpp"
#include "FireballSpell.hpp"
#include "FireballProjectile.hpp"
#include "Enemy.hpp"
#include "Soldier.hpp"
#include "GameData.hpp"
#include "Util/Logger.hpp"

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
            auto& gd = GameData::GetInstance();
            int rfLevel = gd.talentLevels[5]; // 獲取等級

            // 使用更新後的精簡函式計算數值並印出 Log
            float finalHP = 30.0f * gd.GetReinforceHP(rfLevel);
            float finalMinDmg = 1.0f * gd.GetReinforceDamage(rfLevel);
            float finalMaxDmg = 3.0f * gd.GetReinforceDamage(rfLevel);

            LOG_INFO("👨‍🌾 Cast Reinforcement [Lv {}] - Soldier HP: {:.1f}, Damage: {:.1f}~{:.1f}, Cooldown: 10s",
                     rfLevel, finalHP, finalMinDmg, finalMaxDmg);

            ReinforcementSpell rs(stageSoldiers); // 乾淨不需要帶參數
            rs.Cast(clickPos);
            m_ReinforceCooldownTimer = 10.0f;
        }
        else if (m_CurrentSelectedSpell == SpellType::FIREBALL) {
            // 🟢 1. 必須先宣告並計算這些變數，下面的 LOG_INFO 才能認得它們！
            auto& gd = GameData::GetInstance();
            int fbLevel = gd.talentLevels[4]; // 4 代表火球術

            float finalDamage = 100.0f * gd.GetFireballDamage(fbLevel);
            float finalRadius = 80.0f * gd.GetFireballRadius(fbLevel);
            float finalCooldown = 15.0f * gd.GetFireballCooldown(fbLevel);


            // 🟢 2. 這時候印出 Log 才會是正確的天賦數值
            LOG_INFO("🔥 Cast Fireball [Lv {}] - Damage: {}, Cooldown: {}s, Radius: {}", fbLevel, finalDamage, finalCooldown, finalRadius);

            // 從螢幕頂部落
            glm::vec2 startPos = {clickPos.x, 800.0f};

            // 🟢 3. 記得把原本硬編碼的 100.0f, 80.0f 換成 final 變數，天賦加成才會生效！
            m_ActiveFireballs.push_back(std::make_shared<FireballProjectile>(startPos, clickPos, finalDamage, finalRadius));

            // 🟢 4. 冷卻時間也要換成天賦縮減後的變數
            m_FireballCooldownTimer = finalCooldown;
        }

        m_CurrentSelectedSpell = SpellType::NONE;
    }

private:
    SpellType m_CurrentSelectedSpell = SpellType::NONE;
    float m_FireballCooldownTimer = 0.0f;
    float m_ReinforceCooldownTimer = 0.0f;
    std::vector<std::shared_ptr<FireballProjectile>> m_ActiveFireballs;
};