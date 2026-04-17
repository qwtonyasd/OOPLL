#include "tower/BombTower.h"
#include "Util/Time.hpp"

/**
 * @brief BombTower 建構子
 * @param pos 塔放置的座標
 */
BombTower::BombTower(glm::vec2 pos)
    : Tower(pos, "../PTSD/assets/sprites/images/BombTower/TowerLevel1/1.png",
            150.0f, 2.5f, 40.0f, 150, Enemy::DamageType::PHYSICAL) {

    // 保留你原本要求的視覺位移量
    m_VisualOffset = 10.0f;

    // 載入 1 到 35 張攻擊動畫路徑
    for (int i = 1; i <= 35; ++i) {
        m_AttackFrames.push_back("../PTSD/assets/sprites/images/BombTower/TowerLevel1/" + std::to_string(i) + ".png");
    }
}

/**
 * @brief 啟動攻擊動畫並鎖定目標
 */
void BombTower::Attack(std::shared_ptr<Enemy> target,
                      std::vector<std::shared_ptr<Enemy>>& allEnemies,
                      std::vector<std::shared_ptr<Projectile>>& projectiles) {

    // 進入攻擊狀態
    m_IsAttacking = true;
    m_HasFired = false;

    // 紀錄當前時間作為動畫起點
    m_AttackStartTime = static_cast<float>(Util::Time::GetElapsedTimeMs()) / 1000.0f;

    // 暫存目標與容器指標，供 UpdateAnimation 使用
    m_CurrentTarget = target;
    m_AllEnemiesRef = &allEnemies;
    m_ProjectilesRef = &projectiles;
}

/**
 * @brief 更新每一幀的動畫與發射邏輯
 */
void BombTower::UpdateAnimation() {
    if (!m_IsAttacking) return;

    float currentTime = static_cast<float>(Util::Time::GetElapsedTimeMs()) / 1000.0f;
    float elapsed = currentTime - m_AttackStartTime;

    // 設定動畫播放速率 (0.04s 一幀)
    int frameIndex = static_cast<int>(elapsed / 0.04f);

    if (frameIndex < static_cast<int>(m_AttackFrames.size())) {
        // 切換圖片
        SetDrawable(std::make_shared<Util::Image>(m_AttackFrames[frameIndex]));

        // --- 核心邏輯：在第 10 幀發射子彈 ---
        if (frameIndex == 10 && !m_HasFired) {
            if (m_CurrentTarget && m_ProjectilesRef && m_AllEnemiesRef) {

                // 1. 計算子彈起點：從塔的中心向上偏移 35 像素
                glm::vec2 firePos = m_Transform.translation;
                firePos.y += 35.0f;

                // 2. 產生 Bomb 物件：直接傳入 m_CurrentTarget 實現動態追蹤
                auto bomb = std::make_shared<Bomb>(
                    firePos,
                    m_CurrentTarget, // 讓子彈在飛行中追蹤這個敵人
                    m_Damage,
                    *m_AllEnemiesRef,
                    m_DamageType
                );

                // 3. 加入子彈容器
                m_ProjectilesRef->push_back(bomb);
                m_HasFired = true; // 確保此輪動畫只發射一顆炸彈
            }
        }
    } else {
        // 動畫結束，換回第一張圖並解除攻擊狀態
        SetDrawable(std::make_shared<Util::Image>(m_AttackFrames[0]));
        m_IsAttacking = false;
    }
}