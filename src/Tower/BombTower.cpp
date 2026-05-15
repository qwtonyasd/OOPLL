#include "tower/BombTower.h"
#include "Util/Time.hpp"
#include "Tower/Projectile/Projectile.hpp"
#include "Util/Logger.hpp"

// 1. 建構子
BombTower::BombTower(glm::vec2 pos)
    : Tower(pos, "../PTSD/assets/sprites/images/BombTower/TowerLevel1/1.png",
            130.0f, 2.5f, 11.0f, 150, Enemy::DamageType::PHYSICAL) {

    m_BombStats = {
        {
            {145.0f, 2.5f, 11.0f, 10, "../PTSD/assets/sprites/images/BombTower/TowerLevel1/1.png"},
            "../PTSD/assets/sprites/images/BombTower/TowerLevel1/",
            "../PTSD/assets/sprites/images/BombTower/bomb/",
            "../PTSD/assets/sprites/images/BombTower/bomb/1.png",
            Projectile::MoveType::PARABOLA_SIMPLE
        },
        {
            {160.0f, 2.3f, 22.0f, 20, "../PTSD/assets/sprites/images/BombTower/TowerLevel2/1.png"},
            "../PTSD/assets/sprites/images/BombTower/TowerLevel2/",
            "../PTSD/assets/sprites/images/BombTower/TowerLevel2/bomb/",
            "../PTSD/assets/sprites/images/BombTower/TowerLevel2/bomb/1.png",
            Projectile::MoveType::PARABOLA_SIMPLE
        },
        {
            {180.0f, 2.1f, 35.0f, 0, "../PTSD/assets/sprites/images/BombTower/TowerLevel3/1.png"},
            "../PTSD/assets/sprites/images/BombTower/TowerLevel3/",
            "../PTSD/assets/sprites/images/BombTower/TowerLevel3/bomb/",
            "../PTSD/assets/sprites/images/BombTower/TowerLevel3/bomb/1.png",
            Projectile::MoveType::PARABOLA_ROTATE // 3等改為會轉頭的類型
        }
    };

    m_VisualOffset = 10.0f;
    ApplyBaseStats(m_BombStats[0]);
    LoadLevelAssets(); // <-- 這裡如果報錯代表下面沒寫 BombTower::
    UpdateCostText();
}

// 2. 解構子 (必須實作)
BombTower::~BombTower() {}

// 3. Draw 實作
void BombTower::Draw() {
    Tower::Draw();
    if (m_Visible && m_Drawable) {
        glm::vec2 originalPos = m_Transform.translation;
        m_Transform.translation.y += 20.0f;
        auto data = Util::ConvertToUniformBufferData(m_Transform, m_Drawable->GetSize(), m_ZIndex);
        m_Drawable->Draw(data);
        m_Transform.translation = originalPos;
    }
}

// 4. Attack 實作
void BombTower::Attack(std::shared_ptr<Enemy> target,
                      std::vector<std::shared_ptr<Enemy>>& allEnemies,
                      std::vector<std::shared_ptr<Projectile>>& projectiles) {
    if (m_IsAttacking) return;
    m_IsAttacking = true;
    m_HasFired = false;
    m_AttackStartTime = static_cast<float>(Util::Time::GetElapsedTimeMs()) / 1000.0f;
    m_CurrentTarget = target;
    m_AllEnemiesRef = &allEnemies;
    m_ProjectilesRef = &projectiles;
}

// 5. UpdateAnimation 實作
void BombTower::UpdateAnimation() {
    if (!m_IsAttacking) return;

    auto& current = m_BombStats[m_Level - 1];
    float elapsed = (static_cast<float>(Util::Time::GetElapsedTimeMs()) / 1000.0f) - m_AttackStartTime;
    int frameIndex = static_cast<int>(elapsed / 0.04f);

    if (frameIndex < static_cast<int>(m_AttackFrames.size())) {
        SetDrawable(std::make_shared<Util::Image>(m_AttackFrames[frameIndex]));

        if (frameIndex == 10 && !m_HasFired) {
            if (m_CurrentTarget && m_ProjectilesRef && m_AllEnemiesRef) {
                glm::vec2 firePos = m_Transform.translation + glm::vec2(0, 35.0f);
                std::vector<std::string> explosionFrames;
                for (int i = 1; i <= 21; ++i) {
                    explosionFrames.push_back(current.explosionFolder + std::to_string(i) + ".png");
                }

                auto bomb = std::make_shared<Projectile>(
                    current.moveType, // 使用數據表設定的移動類型
                    Projectile::HitType::AREA,
                    firePos, m_CurrentTarget, m_Damage, 0.8f, 80.0f,
                    current.bulletSprite, explosionFrames, m_AllEnemiesRef
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

// 6. Upgrade 實作
void BombTower::Upgrade() {
    if (m_Level < m_MaxLevel) {
        m_Level++;
        ApplyBaseStats(m_BombStats[m_Level - 1]);
        LoadLevelAssets(); // 更新動畫路徑
        LOG_INFO("Bomb Tower Upgraded to Level {}", m_Level);
    }
}

// 7. LoadLevelAssets 實作
void BombTower::LoadLevelAssets() {
    m_AttackFrames.clear();
    auto& current = m_BombStats[m_Level - 1];
    for (int i = 1; i <= 35; ++i) {
        m_AttackFrames.push_back(current.towerAnimFolder + std::to_string(i) + ".png");
    }
    SetDrawable(std::make_shared<Util::Image>(m_AttackFrames[0]));
}