#include "tower/ArcherTower.h"
#include "Util/Time.hpp"
#include "Util/Image.hpp"
#include "Util/GameObject.hpp"
#include <glm/glm.hpp>

ArcherTower::ArcherTower(glm::vec2 pos)
    : Tower(pos, "../PTSD/assets/sprites/images/ArcherTower/TowerLevel1/3.png",
            150.0f, 0.8f, 5.0f, 70, Enemy::DamageType::PHYSICAL) {

    m_VisualOffset = 18.0f; // 站在塔頂的高度

    // 1. 載入底座
    m_BaseDrawable = std::make_shared<Util::Image>("../PTSD/assets/sprites/images/ArcherTower/TowerLevel1/2.png");

    m_AttackFramesRight.clear();
    for (int i = 3; i <= 7; ++i) m_AttackFramesRight.push_back("../PTSD/assets/sprites/images/ArcherTower/TowerLevel1/" + std::to_string(i) + ".png");
    m_AttackFramesLeft.clear();
    for (int i = 8; i <= 12; ++i) m_AttackFramesLeft.push_back("../PTSD/assets/sprites/images/ArcherTower/TowerLevel1/" + std::to_string(i) + ".png");

    // 初始狀態：兩個人都先顯示第一幀
    m_LeftDrawable = std::make_shared<Util::Image>(m_AttackFramesRight[0]);
    m_RightDrawable = std::make_shared<Util::Image>(m_AttackFramesRight[0]);
    m_NextShooterLeft = true;
}

void ArcherTower::Attack(std::shared_ptr<Enemy> target,
                        std::vector<std::shared_ptr<Enemy>>& allEnemies,
                        std::vector<std::shared_ptr<Projectile>>& projectiles) {
    (void)allEnemies;
    float currentTime = static_cast<float>(Util::Time::GetElapsedTimeMs()) / 1000.0f;

    // 檢查冷卻 (這裡假設 0.8s 射一次，兩個人輪流所以體感是 0.4s 射一發)
    if (currentTime - m_LastAttackTime < m_AttackRate / 2.0f) return;

    m_CurrentTarget = target;
    m_ProjectilesRef = &projectiles;

    // 輪流啟動動畫
    if (m_NextShooterLeft && !m_IsLeftAttacking) {
        m_IsLeftAttacking = true;
        m_LeftHasFired = false;
        m_LeftAttackStartTime = currentTime;
        m_NextShooterLeft = false; // 下次換右邊
        m_LastAttackTime = currentTime;
    }
    else if (!m_NextShooterLeft && !m_IsRightAttacking) {
        m_IsRightAttacking = true;
        m_RightHasFired = false;
        m_RightAttackStartTime = currentTime;
        m_NextShooterLeft = true; // 下次換左邊
        m_LastAttackTime = currentTime;
    }
}

void ArcherTower::UpdateAnimation() {
    float currentTime = static_cast<float>(Util::Time::GetElapsedTimeMs()) / 1000.0f;
    bool targetIsAbove = (m_CurrentTarget && m_CurrentTarget->GetPosition().y >= m_Transform.translation.y);
    auto& currentFrames = targetIsAbove ? m_AttackFramesLeft : m_AttackFramesRight;

    // --- 獨立處理左弓箭手 ---
    if (m_IsLeftAttacking) {
        int frame = static_cast<int>((currentTime - m_LeftAttackStartTime) / 0.05f);
        if (frame < static_cast<int>(currentFrames.size())) {
            m_LeftDrawable = std::make_shared<Util::Image>(currentFrames[frame]);
            if (frame == 3 && !m_LeftHasFired) {
                // 從左方偏移 (-8, 18) 射出
                FireArrow(glm::vec2(-8.0f, m_VisualOffset));
                m_LeftHasFired = true;
            }
        } else {
            m_IsLeftAttacking = false; // 動畫結束
        }
    } else {
        // 沒攻擊時顯示待機圖
        m_LeftDrawable = std::make_shared<Util::Image>(currentFrames[0]);
    }

    // --- 獨立處理右弓箭手 ---
    if (m_IsRightAttacking) {
        int frame = static_cast<int>((currentTime - m_RightAttackStartTime) / 0.05f);
        if (frame < static_cast<int>(currentFrames.size())) {
            m_RightDrawable = std::make_shared<Util::Image>(currentFrames[frame]);
            if (frame == 3 && !m_RightHasFired) {
                // 從右方偏移 (8, 18) 射出
                FireArrow(glm::vec2(8.0f, m_VisualOffset));
                m_RightHasFired = true;
            }
        } else {
            m_IsRightAttacking = false; // 動畫結束
        }
    } else {
        // 沒攻擊時顯示待機圖
        m_RightDrawable = std::make_shared<Util::Image>(currentFrames[0]);
    }
}

void ArcherTower::FireArrow(glm::vec2 offset) {
    if (m_CurrentTarget && m_ProjectilesRef) {
        // 這裡會精準計算每位弓箭手的手部位置
        glm::vec2 firePos = m_Transform.translation + offset;
        auto arrow = std::make_shared<Arrow>(firePos, m_CurrentTarget, m_Damage, m_DamageType);
        m_ProjectilesRef->push_back(arrow);
    }
}

void ArcherTower::Draw() {
    // --- 新增：先跑父類別邏輯 (畫範圍圈與選單) ---
    Tower::Draw();

    // 備份原始座標
    glm::vec2 originalPos = m_Transform.translation;
    float originalZ = m_ZIndex;

    // ... 接下來是你原本畫底座與小人的代碼 ...
    // 1. 畫底座
    if (m_BaseDrawable) {
        m_Drawable = m_BaseDrawable;
        // 注意：因為 PTSD 框架的 GameObject::Draw() 只會畫目前 m_Drawable 指定的東西
        // 所以這裡維持呼叫 Util::GameObject::Draw() 是對的
        Util::GameObject::Draw();
    }

    // 2. 畫左小人 (偏移到左上)
    if (m_LeftDrawable) {
        m_Drawable = m_LeftDrawable;
        m_Transform.translation = originalPos + glm::vec2(-8.0f, m_VisualOffset);
        m_ZIndex = originalZ + 0.1f;
        Util::GameObject::Draw();
    }

    // 3. 畫右小人 (偏移到右上)
    if (m_RightDrawable) {
        m_Drawable = m_RightDrawable;
        m_Transform.translation = originalPos + glm::vec2(8.0f, m_VisualOffset);
        m_ZIndex = originalZ + 0.11f;
        Util::GameObject::Draw();
    }

    // 恢復座標，避免影響其他邏輯
    m_Transform.translation = originalPos;
    m_ZIndex = originalZ;
}