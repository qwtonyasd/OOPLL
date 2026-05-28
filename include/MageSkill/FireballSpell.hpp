#pragma once
#include "Spell.hpp"
#include "Enemy.hpp"
#include <vector>
#include <cmath>

class FireballSpell : public Spell {
private:
    std::vector<std::shared_ptr<Enemy>>& m_StageEnemies;
    float m_Damage = 100.0f;
    float m_Radius = 80.0f; // 爆炸半徑

public:
    FireballSpell(std::vector<std::shared_ptr<Enemy>>& stageEnemies)
        : Spell(20.0f, "../PTSD/assets/sprites/images/Spells/fireball_icon.png"), 
          m_StageEnemies(stageEnemies) {}

    void Cast(glm::vec2 clickPos) override {
        if (!IsReady()) return;

        // 1. 觸發火球術的視覺特效（你可以新增一個 EffectManager 來播爆炸動畫）
        // EffectManager::Spawn("FireballExplosion", clickPos);

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