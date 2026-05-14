#include "tower/BombTower.h"
#include "Util/Time.hpp"
#include "Tower/Projectile/Projectile.hpp"

BombTower::BombTower(glm::vec2 pos)
    : Tower(pos, "../PTSD/assets/sprites/images/BombTower/TowerLevel1/1.png",
            150.0f, 2.5f, 11.0f, 150, Enemy::DamageType::PHYSICAL) {

    m_VisualOffset = 10.0f;

    for (int i = 1; i <= 35; ++i) {
        m_AttackFrames.push_back("../PTSD/assets/sprites/images/BombTower/TowerLevel1/" + std::to_string(i) + ".png");
    }
}

void BombTower::Attack(std::shared_ptr<Enemy> target,
                      std::vector<std::shared_ptr<Enemy>>& allEnemies,
                      std::vector<std::shared_ptr<Projectile>>& projectiles) {
    m_IsAttacking = true;
    m_HasFired = false;
    m_AttackStartTime = static_cast<float>(Util::Time::GetElapsedTimeMs()) / 1000.0f;

    m_CurrentTarget = target;
    m_AllEnemiesRef = &allEnemies;
    m_ProjectilesRef = &projectiles;
}

void BombTower::UpdateAnimation() {
    if (!m_IsAttacking) return;

    float currentTime = static_cast<float>(Util::Time::GetElapsedTimeMs()) / 1000.0f;
    float elapsed = currentTime - m_AttackStartTime;
    int frameIndex = static_cast<int>(elapsed / 0.04f);

    if (frameIndex < static_cast<int>(m_AttackFrames.size())) {
        SetDrawable(std::make_shared<Util::Image>(m_AttackFrames[frameIndex]));

        // --- 核心修正：發射萬用投射物 ---
        if (frameIndex == 10 && !m_HasFired) {
            if (m_CurrentTarget && m_ProjectilesRef && m_AllEnemiesRef) {

                glm::vec2 firePos = m_Transform.translation + glm::vec2(0, 35.0f);

                // 準備爆炸特效路徑 (D 類)
                std::vector<std::string> explosionFrames;
                for (int i = 1; i <= 21; ++i) {
                    explosionFrames.push_back("../PTSD/assets/sprites/images/BombTower/bomb/" + std::to_string(i) + ".png");
                }

                // 建立 Projectile：B類移動 + D類命中
                auto bomb = std::make_shared<Projectile>(
                    Projectile::MoveType::PARABOLA_SIMPLE, // B 類：簡單拋物線，不旋轉
                    Projectile::HitType::AREA,             // D 類：範圍爆炸傷害
                    firePos,
                    m_CurrentTarget,
                    m_Damage,
                    0.8f,  // 飛行時間
                    80.0f, // 拋物線高度
                    "../PTSD/assets/sprites/images/BombTower/bomb/1.png", // 初始炸彈圖
                    explosionFrames,
                    m_AllEnemiesRef // 傳入敵人清單指標以計算 AOE
                );

                m_ProjectilesRef->push_back(bomb);
                m_HasFired = true;
            }
        }
    } else {
        SetDrawable(std::make_shared<Util::Image>(m_AttackFrames[0]));
        m_IsAttacking = false;
    }
}

void BombTower::Draw() {
    Tower::Draw();
    if (m_Visible && m_Drawable) {
        glm::vec2 originalPos = m_Transform.translation;
        m_Transform.translation.y += 20.0f; // 視覺修正

        auto data = Util::ConvertToUniformBufferData(m_Transform, m_Drawable->GetSize(), m_ZIndex);
        m_Drawable->Draw(data);

        m_Transform.translation = originalPos;
    }
}

void BombTower::Upgrade() {
    // 留空即可，目的是滿足編譯器的要求
}