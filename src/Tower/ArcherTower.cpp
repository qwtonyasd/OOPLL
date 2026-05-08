#include "tower/ArcherTower.h"
#include "Util/Time.hpp"

ArcherTower::ArcherTower(glm::vec2 pos)
    : Tower(pos, "../PTSD/assets/sprites/images/ArcherTower/TowerLevel1/1.png",
            175.0f, 0.8f, 15.0f, 70, Enemy::DamageType::PHYSICAL) {

    // 1. 定義三級數據：{範圍, 攻速, 傷害, 升級費用, 基座圖路徑}
    m_ArcherStats = {
        {175.0f, 0.8f, 15.0f, 70,  "../PTSD/assets/sprites/images/ArcherTower/TowerLevel1/1.png"},
        {200.0f, 0.7f, 25.0f, 110, "../PTSD/assets/sprites/images/ArcherTower/TowerLevel2/1.png"},
        {225.0f, 0.6f, 40.0f, 160, "../PTSD/assets/sprites/images/ArcherTower/TowerLevel3/1.png"}
    };

    // 2. 載入等級 1 的資源
    LoadLevelAssets();
}

void ArcherTower::LoadLevelAssets() {
    m_UpFrames.clear();
    m_DownFrames.clear();

    // 根據當前 m_Level 組出資料夾路徑 (例如 TowerLevel1, TowerLevel2...)
    std::string folder = "TowerLevel" + std::to_string(m_Level);
    std::string basePath = "../PTSD/assets/sprites/images/ArcherTower/" + folder + "/";

    for (int i = 1; i <= 5; ++i) {
        m_UpFrames.push_back(basePath + "Up/" + std::to_string(i) + ".png");
        m_DownFrames.push_back(basePath + "Down/" + std::to_string(i) + ".png");
    }

    // 更新初始顯示圖案 (避免升級瞬間閃爍舊圖)
    m_LeftDrawable = std::make_shared<Util::Image>(m_DownFrames[0]);
    m_RightDrawable = std::make_shared<Util::Image>(m_DownFrames[0]);
}

void ArcherTower::Upgrade() {
    if (m_Level < m_MaxLevel) {
        m_Level++;

        // 1. 套用新等級的數值 (Range, Cooldown, Damage, Base Image)
        ApplyBaseStats(m_ArcherStats[m_Level - 1]);

        // 2. 重新載入弓箭手動畫圖案
        LoadLevelAssets();

        LOG_DEBUG("Archer Tower upgraded to Level {}", m_Level);
    }
}

void ArcherTower::Attack(std::shared_ptr<Enemy> target,
                        [[maybe_unused]] std::vector<std::shared_ptr<Enemy>>& allEnemies,
                        std::vector<std::shared_ptr<Projectile>>& projectiles) {
    m_CurrentTarget = target;
    m_ProjectilesRef = &projectiles;
    float currentTime = static_cast<float>(Util::Time::GetElapsedTimeMs()) / 1000.0f;

    bool targetIsUp = (target->GetPosition().y > m_Transform.translation.y);

    if (m_NextShooterLeft) {
        if (!m_IsLeftAttacking) {
            m_IsLeftAttacking = true;
            m_LeftHasFired = false;
            m_LeftAttackStartTime = currentTime;
            m_LeftShootUp = targetIsUp;
            m_NextShooterLeft = false;
        }
    } else {
        if (!m_IsRightAttacking) {
            m_IsRightAttacking = true;
            m_RightHasFired = false;
            m_RightAttackStartTime = currentTime;
            m_RightShootUp = targetIsUp;
            m_NextShooterLeft = true;
        }
    }
}

void ArcherTower::UpdateAnimation() {
    float currentTime = static_cast<float>(Util::Time::GetElapsedTimeMs()) / 1000.0f;
    const float frameDuration = 0.05f;

    auto updateArcher = [&](float startTime, bool& hasFired, bool isUp,
                            std::shared_ptr<Core::Drawable>& drawable,
                            bool& attackingState, glm::vec2 offset) {

        float elapsed = currentTime - startTime;
        int frame = static_cast<int>(elapsed / frameDuration);

        const std::vector<std::string>& currentFrames = isUp ? m_UpFrames : m_DownFrames;

        if (frame < 5) {
            drawable = std::make_shared<Util::Image>(currentFrames[frame]);

            if (frame == 3 && !hasFired && m_CurrentTarget && m_ProjectilesRef) {
                auto arrow = std::make_shared<Projectile>(
                    Projectile::MoveType::PARABOLA_ROTATE,
                    Projectile::HitType::SINGLE,
                    m_Transform.translation + offset, m_CurrentTarget, m_Damage,
                    0.5f, 40.0f,
                    "../PTSD/assets/sprites/images/ArcherTower/Arrow/1.png",
                    std::vector<std::string>{}
                );
                m_ProjectilesRef->push_back(arrow);
                hasFired = true;
            }
        } else {
            attackingState = false;
            drawable = std::make_shared<Util::Image>(currentFrames[0]);
        }
    };

    if (m_IsLeftAttacking) {
        updateArcher(m_LeftAttackStartTime, m_LeftHasFired, m_LeftShootUp,
                     m_LeftDrawable, m_IsLeftAttacking, glm::vec2(-8, 25));
    }
    if (m_IsRightAttacking) {
        updateArcher(m_RightAttackStartTime, m_RightHasFired, m_RightShootUp,
                     m_RightDrawable, m_IsRightAttacking, glm::vec2(8, 25));
    }
}

void ArcherTower::Draw() {
    Tower::Draw();

    if (m_Visible) {
        float yOffset = 15.0f;
        glm::vec2 adjustedPos = m_Transform.translation + glm::vec2(0, yOffset);

        if (m_Drawable) {
            m_Drawable->Draw(Util::ConvertToUniformBufferData(
                Util::Transform{adjustedPos, 0, m_Transform.scale},
                m_Drawable->GetSize(),
                m_ZIndex
            ));
        }

        Util::Transform leftTransform = m_Transform;
        leftTransform.translation = adjustedPos + glm::vec2(-8, 17);
        m_LeftDrawable->Draw(Util::ConvertToUniformBufferData(
            leftTransform, m_LeftDrawable->GetSize(), m_ZIndex + 0.1f
        ));

        Util::Transform rightTransform = m_Transform;
        rightTransform.translation = adjustedPos + glm::vec2(8, 17);
        m_RightDrawable->Draw(Util::ConvertToUniformBufferData(
            rightTransform, m_RightDrawable->GetSize(), m_ZIndex + 0.1f
        ));
    }
}