#include "tower/MageTower.h"
#include "Util/Time.hpp"
#include <cmath>

MageTower::MageTower(glm::vec2 pos)
    : Tower(pos, "../PTSD/assets/sprites/images/MageTower/TowerLevel1/1.png",
            120.0f, 1.5f, 13.0f, 100, Enemy::DamageType::MAGIC) {

    m_MageStats = {
        {120.0f, 1.5f, 13.0f, 110, "../PTSD/assets/sprites/images/MageTower/TowerLevel1/1.png"},
        {140.0f, 1.4f, 25.0f, 240, "../PTSD/assets/sprites/images/MageTower/TowerLevel2/1.png"},
        {160.0f, 1.2f, 45.0f, 0,   "../PTSD/assets/sprites/images/MageTower/TowerLevel3/1.png"}
    };

    ApplyBaseStats(m_MageStats[0]);
    LoadLevelAssets();
}

void MageTower::LoadLevelAssets() {
    m_BaseFrames.clear();
    m_AttackUpFrames.clear();
    m_AttackDownFrames.clear();

    std::string folder = "TowerLevel" + std::to_string(m_Level);
    std::string basePath = "../PTSD/assets/sprites/images/MageTower/" + folder + "/";

    m_BulletSprite = "../PTSD/assets/sprites/images/MageTower/Mage/1.png";
    m_HitEffectFolder = "../PTSD/assets/sprites/images/MageTower/Mage/";

    // 載入基座 1-6.png (引用自)
    for (int i = 1; i <= 6; ++i) {
        m_BaseFrames.push_back(basePath + std::to_string(i) + ".png");
    }

    // 載入法師 Up/Down 動畫 (引用自)
    for (int i = 1; i <= 7; ++i) {
        m_AttackUpFrames.push_back(basePath + "Up/" + std::to_string(i) + ".png");
        m_AttackDownFrames.push_back(basePath + "Down/" + std::to_string(i) + ".png");
    }

    SetDrawable(std::make_shared<Util::Image>(m_AttackUpFrames[0]));
}

void MageTower::UpdateAnimation() {
    if (!m_IsAttacking) {
        auto& idleFrames = m_IsTargetAbove ? m_AttackUpFrames : m_AttackDownFrames;
        SetDrawable(std::make_shared<Util::Image>(idleFrames[0]));
        return;
    }

    float currentTime = static_cast<float>(Util::Time::GetElapsedTimeMs()) / 1000.0f;
    float elapsed = currentTime - m_AttackStartTime;

    // 控制動畫播放速度，這裡設為每幀 0.08 秒
    int frameIndex = static_cast<int>(elapsed / 0.08f);
    auto& currentFrames = m_IsTargetAbove ? m_AttackUpFrames : m_AttackDownFrames;

    if (frameIndex < static_cast<int>(currentFrames.size())) {
        SetDrawable(std::make_shared<Util::Image>(currentFrames[frameIndex]));

        // 第 5 幀（frameIndex == 4）時射出子彈
        if (frameIndex == 4 && !m_HasFired) {
            if (m_CurrentTarget && m_ProjectilesRef) {
                glm::vec2 firePos = m_Transform.translation + glm::vec2(0, 25.0f);
                std::vector<std::string> hitFrames;
                for (int i = 2; i <= 9; ++i) hitFrames.push_back(m_HitEffectFolder + std::to_string(i) + ".png");

                auto bolt = std::make_shared<Projectile>(
                    Projectile::MoveType::STRAIGHT, Projectile::HitType::SINGLE,
                    firePos, m_CurrentTarget, m_Damage, 0.35f, 0.0f, m_BulletSprite, hitFrames
                );
                m_ProjectilesRef->push_back(bolt);
                m_HasFired = true;
            }
        }
    } else {
        // 動畫結束，重置狀態
        m_IsAttacking = false;
    }
}

void MageTower::Draw() {
    Tower::Draw();

    if (m_Visible) {
        // 1. 基本座標取整 (Pixel Snapping)
        Util::Transform pixelTransform = m_Transform;
        pixelTransform.translation.x = std::round(m_Transform.translation.x);
        pixelTransform.translation.y = std::round(m_Transform.translation.y);
        pixelTransform.scale = glm::vec2(1.0f, 1.0f);

        // 2. 確定基座目前要畫哪一幀
        int baseFrameIndex = 0;
        if (m_IsAttacking) {
            float elapsed = (static_cast<float>(Util::Time::GetElapsedTimeMs()) / 1000.0f) - m_AttackStartTime;
            // 控制基座動畫速度，這裡設為 0.08s 一幀
            baseFrameIndex = static_cast<int>(elapsed / 0.08f) % m_BaseFrames.size();
        }

        auto baseDrawable = std::make_shared<Util::Image>(m_BaseFrames[baseFrameIndex]);
        glm::vec2 imgSize = baseDrawable->GetSize();

        // --- 核心修正：解決奇數像素模糊 ---
        // 將 float 轉為 int 後再取餘數
        if (static_cast<int>(imgSize.x) % 2 != 0) {
            pixelTransform.translation.x += 0.5f;
        }
        if (static_cast<int>(imgSize.y) % 2 != 0) {
            pixelTransform.translation.y += 0.5f;
        }

        auto baseData = Util::ConvertToUniformBufferData(pixelTransform, imgSize, m_ZIndex);
        baseDrawable->Draw(baseData);

        // 3. 繪製法師
        if (m_Drawable) {
            Util::Transform mageTransform = pixelTransform;
            // 這裡的位移也要確保是整數
            mageTransform.translation.y += 18.0f;

            // 如果法師圖片本身也是奇數，這裡也要補 0.5f
            glm::vec2 mageSize = m_Drawable->GetSize();
            if (static_cast<int>(mageSize.x) % 2 != 0) mageTransform.translation.x += 0.5f;
            // 注意：如果上面基座已經補過 0.5 了，這裡可能要視情況微調

            auto mageData = Util::ConvertToUniformBufferData(mageTransform, mageSize, m_ZIndex + 0.1f);
            m_Drawable->Draw(mageData);
        }
    }
}

void MageTower::Attack(std::shared_ptr<Enemy> target,
                      [[maybe_unused]] std::vector<std::shared_ptr<Enemy>>& enemies,
                      std::vector<std::shared_ptr<Projectile>>& projectiles) {
    if (target && !m_IsAttacking) {
        m_IsAttacking = true;
        m_HasFired = false;
        m_AttackStartTime = static_cast<float>(Util::Time::GetElapsedTimeMs()) / 1000.0f;
        m_CurrentTarget = target;
        m_ProjectilesRef = &projectiles;
        // 判斷法師該看哪邊
        m_IsTargetAbove = (m_CurrentTarget->GetPosition().y > m_Transform.translation.y);
    }
}

void MageTower::Upgrade() {
    if (m_Level < m_MaxLevel) {
        m_Level++;
        ApplyBaseStats(m_MageStats[m_Level - 1]);
        LoadLevelAssets();
        UpdateCostText();
    }
}