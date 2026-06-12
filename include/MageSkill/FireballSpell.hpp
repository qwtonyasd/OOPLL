#pragma once
#include "Spell.hpp"
#include "Enemy.hpp"
#include <vector>
#include <cmath>
#include "GameData.hpp"

class FireballSpell : public Spell {
private:
    std::vector<std::shared_ptr<Enemy>>& m_StageEnemies;
    float m_Damage = 100.0f;
    float m_Radius = 80.0f; // 爆炸半徑

public:
    FireballSpell(std::vector<std::shared_ptr<Enemy>>& stageEnemies)
        : Spell(20.0f, "../PTSD/assets/sprites/images/Spells/fireball_icon.png"), 
          m_StageEnemies(stageEnemies) {
        // 🆕 【核心改動】在關卡初始化時，根據遊戲外的天賦等級調整數值
        auto& gd = GameData::GetInstance();
        int fbLevel = gd.talentLevels[4]; // 4 代表火球術

        // 1. 調整傷害與半徑
        m_Damage = 100.0f * gd.GetFireballDamage(fbLevel);
        m_Radius = 80.0f * gd.GetFireballRadius(fbLevel);

    }

    void Cast(glm::vec2 clickPos) override {
        if (!IsReady()) return;


        // 2. 核心邏輯：範圍內的所有怪物通通挨打！
        for (auto& enemy : m_StageEnemies) {
            if (!enemy || enemy->GetHP() <= 0) continue;

            // 計算怪物與點擊中心的距離
            float dist = glm::distance(clickPos, enemy->GetPosition());
            if (dist <= m_Radius) {
                // 可以做傷害衰減：離中心越近越痛，邊緣比較不痛
                float damageFactor = 1.0f - (dist / m_Radius) * 0.3f; // 最外圈拿 70% 傷害
                enemy->TakeDamage(m_Damage * damageFactor);
            }
        }

        StartCooldown();
    }
};