#include "tower/MageTower.h"
#include "Util/Time.hpp"

MageTower::MageTower(glm::vec2 pos)
    : Tower(pos, "../PTSD/assets/sprites/images/MageTower/TowerLevel1/1.png",
            120.0f, 1.5f, 13.0f, 100, Enemy::DamageType::MAGIC) {

    // 假設魔法塔攻擊動畫有 10 張（可依據你的資源數量修改）
    for (int i = 1; i <= 15; ++i) {
        m_AttackFrames.push_back("../PTSD/assets/sprites/images/MageTower/TowerLevel1/" + std::to_string(i) + ".png");
    }
}

void MageTower::Attack(std::shared_ptr<Enemy> target,
                      std::vector<std::shared_ptr<Enemy>>& enemies,
                      std::vector<std::shared_ptr<Projectile>>& projectiles) {
    (void)enemies; // 魔法塔通常是單體攻擊，暫不使用 enemies 容器

    if (target) {
        m_IsAttacking = true;
        m_HasFired = false;
        m_AttackStartTime = static_cast<float>(Util::Time::GetElapsedTimeMs()) / 1000.0f;

        // 鎖定目標與容器
        m_CurrentTarget = target;
        m_ProjectilesRef = &projectiles;
    }
}

void MageTower::UpdateAnimation() {
    if (!m_IsAttacking) return;

    float currentTime = static_cast<float>(Util::Time::GetElapsedTimeMs()) / 1000.0f;
    float elapsed = currentTime - m_AttackStartTime;

    // 法師動作通常快一點，這裡設 0.06s 換一幀
    int frameIndex = static_cast<int>(elapsed / 0.04f);

    if (frameIndex < static_cast<int>(m_AttackFrames.size())) {
        SetDrawable(std::make_shared<Util::Image>(m_AttackFrames[frameIndex]));

        // 假設在第 7 幀（動作後半段）射出魔法彈
        if (frameIndex == 11 && !m_HasFired) {
            if (m_CurrentTarget && m_ProjectilesRef) {
                // 魔法彈起點向上偏移，從法杖頂端射出
                glm::vec2 firePos = m_Transform.translation;
                firePos.y += 22.0f;

                auto bolt = std::make_shared<MagicBolt>(
                    firePos,
                    m_CurrentTarget,
                    m_Damage,
                    m_DamageType
                );
                m_ProjectilesRef->push_back(bolt);
                m_HasFired = true;
            }
        }
    } else {
        // 播完換回第一張
        SetDrawable(std::make_shared<Util::Image>(m_AttackFrames[0]));
        m_IsAttacking = false;
    }
}