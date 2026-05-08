#include "tower/MageTower.h"
#include "Util/Time.hpp"
#include "Tower/Projectlie/Projectile.hpp"

MageTower::MageTower(glm::vec2 pos)
    : Tower(pos, "../PTSD/assets/sprites/images/MageTower/TowerLevel1/1.png",
            120.0f, 1.5f, 13.0f, 100, Enemy::DamageType::MAGIC) {

    for (int i = 1; i <= 15; ++i) {
        m_AttackFrames.push_back("../PTSD/assets/sprites/images/MageTower/TowerLevel1/" + std::to_string(i) + ".png");
    }
}

void MageTower::Attack(std::shared_ptr<Enemy> target,
                      std::vector<std::shared_ptr<Enemy>>& enemies,
                      std::vector<std::shared_ptr<Projectile>>& projectiles) {
    (void)enemies;

    if (target) {
        m_IsAttacking = true;
        m_HasFired = false;
        m_AttackStartTime = static_cast<float>(Util::Time::GetElapsedTimeMs()) / 1000.0f;
        m_CurrentTarget = target;
        m_ProjectilesRef = &projectiles;
    }
}

void MageTower::UpdateAnimation() {
    if (!m_IsAttacking) return;

    float currentTime = static_cast<float>(Util::Time::GetElapsedTimeMs()) / 1000.0f;
    float elapsed = currentTime - m_AttackStartTime;
    int frameIndex = static_cast<int>(elapsed / 0.04f);

    if (frameIndex < static_cast<int>(m_AttackFrames.size())) {
        SetDrawable(std::make_shared<Util::Image>(m_AttackFrames[frameIndex]));

        // --- 核心修正：發射萬用投射物 ---
        if (frameIndex == 11 && !m_HasFired) {
            if (m_CurrentTarget && m_ProjectilesRef) {

                glm::vec2 firePos = m_Transform.translation + glm::vec2(0, 22.0f);

                // 準備命中特效路徑 (E 類：單體命中)
                std::vector<std::string> hitFrames;
                for (int i = 2; i <= 9; ++i) {
                    hitFrames.push_back("../PTSD/assets/sprites/images/MageTower/Mage/" + std::to_string(i) + ".png");
                }

                // 建立 Projectile：C類移動 + E類命中
                auto bolt = std::make_shared<Projectile>(
                    Projectile::MoveType::STRAIGHT,  // C 類：直線飛行
                    Projectile::HitType::SINGLE,      // E 類：單體傷害
                    firePos,
                    m_CurrentTarget,
                    m_Damage,
                    0.4f,  // 魔法彈飛得快，設定短時間
                    0.0f,  // 直線移動，弧度設為 0
                    "../PTSD/assets/sprites/images/MageTower/Mage/1.png", // 初始子彈圖
                    hitFrames
                );

                m_ProjectilesRef->push_back(bolt);
                m_HasFired = true;
            }
        }
    } else {
        SetDrawable(std::make_shared<Util::Image>(m_AttackFrames[0]));
        m_IsAttacking = false;
    }
}

void MageTower::Draw() {
    Tower::Draw();
    if (m_Visible && m_Drawable) {
        glm::vec2 originalPos = m_Transform.translation;
        m_Transform.translation.y += 10.0f;

        auto data = Util::ConvertToUniformBufferData(m_Transform, m_Drawable->GetSize(), m_ZIndex);
        m_Drawable->Draw(data);

        m_Transform.translation = originalPos;
    }
}

void MageTower::Upgrade() {
    // 留空即可，目的是滿足編譯器的要求
}