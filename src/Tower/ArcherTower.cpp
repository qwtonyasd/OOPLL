#include "tower/ArcherTower.h"
#include "Util/Time.hpp"

ArcherTower::ArcherTower(glm::vec2 pos)
    : Tower(pos, "../PTSD/assets/sprites/images/ArcherTower/TowerLevel1/1.png",
            175.0f, 0.8f, 5.0f, 70, Enemy::DamageType::PHYSICAL) {

    // 1. 定義三級數據：{範圍, 攻速, 傷害, 升級費用, 基座圖路徑}
    m_ArcherStats = {
        {175.0f, 0.8f, 9.0f, 10,  "../PTSD/assets/sprites/images/ArcherTower/TowerLevel1/1.png"},
        {200.0f, 0.7f, 25.0f, 10, "../PTSD/assets/sprites/images/ArcherTower/TowerLevel2/1.png"},
        {225.0f, 0.6f, 40.0f, 160, "../PTSD/assets/sprites/images/ArcherTower/TowerLevel3/1.png"}
    };
    //讓價格正常顯示
    ApplyBaseStats(m_ArcherStats[0]);

    // 2. 載入等級 1 的資源
    LoadLevelAssets();

    UpdateCostText();
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
        // 套用下一級數值
        ApplyBaseStats(m_ArcherStats[m_Level - 1]);
        // 重新載入對應等級的資源
        LoadLevelAssets();
        // 重要：更新文字顯示為「再下一級」的價格
        UpdateCostText();
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

        // --- 核心修正：基礎座標取整 ---
        Util::Transform snappedBase = m_Transform;
        snappedBase.translation.x = std::round(m_Transform.translation.x);
        snappedBase.translation.y = std::round(m_Transform.translation.y);
        snappedBase.scale = glm::vec2(1.0f, 1.0f); // 強制 1:1 縮放

        glm::vec2 adjustedPos = snappedBase.translation + glm::vec2(0, yOffset);

        // 1. 繪製基座
        if (m_Drawable) {
            glm::vec2 baseSize = m_Drawable->GetSize();
            Util::Transform baseTransform = snappedBase;
            baseTransform.translation = adjustedPos;

            // 檢查奇數像素補償
            if (static_cast<int>(baseSize.x) % 2 != 0) baseTransform.translation.x += 0.5f;
            if (static_cast<int>(baseSize.y) % 2 != 0) baseTransform.translation.y += 0.5f;

            m_Drawable->Draw(Util::ConvertToUniformBufferData(
                baseTransform, baseSize, m_ZIndex
            ));
        }

        // 2. 繪製左側弓箭手
        if (m_LeftDrawable) {
            glm::vec2 leftSize = m_LeftDrawable->GetSize();
            Util::Transform leftTransform = snappedBase;
            // 這裡的位移量 (-8, 17) 是整數，所以可以直接加
            leftTransform.translation = adjustedPos + glm::vec2(-8, 17);

            // 檢查奇數像素補償
            if (static_cast<int>(leftSize.x) % 2 != 0) leftTransform.translation.x += 0.5f;
            if (static_cast<int>(leftSize.y) % 2 != 0) leftTransform.translation.y += 0.5f;

            m_LeftDrawable->Draw(Util::ConvertToUniformBufferData(
                leftTransform, leftSize, m_ZIndex + 0.1f
            ));
        }

        // 3. 繪製右側弓箭手
        if (m_RightDrawable) {
            glm::vec2 rightSize = m_RightDrawable->GetSize();
            Util::Transform rightTransform = snappedBase;
            rightTransform.translation = adjustedPos + glm::vec2(8, 17);

            // 檢查奇數像素補償
            if (static_cast<int>(rightSize.x) % 2 != 0) rightTransform.translation.x += 0.5f;
            if (static_cast<int>(rightSize.y) % 2 != 0) rightTransform.translation.y += 0.5f;

            m_RightDrawable->Draw(Util::ConvertToUniformBufferData(
                rightTransform, rightSize, m_ZIndex + 0.1f
            ));
        }
    }
}