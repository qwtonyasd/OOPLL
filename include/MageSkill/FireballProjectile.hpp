#pragma once
#include "Util/GameObject.hpp" // 必須包含這個，且路徑要正確
#include "Enemy.hpp"
#include <memory>
#include <vector>

// 確保你是在正確的命名空間下（如果有的話）
class FireballProjectile : public Util::GameObject {
private:
    glm::vec2 m_StartPos;
    glm::vec2 m_TargetPos;
    float m_Progress = 0.0f;
    float m_Speed = 2.0f;
    float m_Damage;
    float m_Radius;

public:
    FireballProjectile(glm::vec2 start, glm::vec2 target, float dmg, float radius)
        : m_StartPos(start), m_TargetPos(target), m_Damage(dmg), m_Radius(radius) {

        // 修正：請確認路徑正確
        SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/Spells/fireball.png"));
        m_Transform.translation = start;
        m_ZIndex = 40.0f;
    }

    // 修正：Update 必須包含 dt 參數
    bool Update(std::vector<std::shared_ptr<Enemy>>& enemies, float dt) {
        m_Progress += dt * m_Speed;
        m_Transform.translation = m_StartPos + (m_TargetPos - m_StartPos) * m_Progress;
        m_Transform.scale = glm::vec2(0.5f + m_Progress);

        if (m_Progress >= 1.0f) {
            Explode(enemies);
            return false;
        }
        return true;
    }

    void Explode(std::vector<std::shared_ptr<Enemy>>& enemies) {
        // 爆炸中心點就是火球當前的位置 (m_Transform.translation)
        glm::vec2 explosionPos = m_Transform.translation;

        for (auto& enemy : enemies) {
            if (!enemy || enemy->GetHP() <= 0) continue;

            // 計算距離
            float dist = glm::distance(explosionPos, enemy->GetPosition());

            // 如果在爆炸半徑內
            if (dist <= m_Radius) {
                // 可選：實作傷害衰減，離中心越近傷害越高
                float damage = m_Damage * (1.0f - (dist / m_Radius) * 0.5f);
                enemy->TakeDamage(damage);
            }
        }
    }
};