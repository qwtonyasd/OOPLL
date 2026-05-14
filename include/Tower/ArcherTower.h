#ifndef ARCHER_TOWER_H
#define ARCHER_TOWER_H

#include "Tower.hpp"
#include "Tower/Projectile/Projectile.hpp"
#include "Util/Image.hpp"
#include "Core/Drawable.hpp"
#include "Util/Logger.hpp"
#include <vector>
#include <string>

class ArcherTower : public Tower {
public:
    ArcherTower(glm::vec2 pos);
    void Attack(std::shared_ptr<Enemy> target,
                std::vector<std::shared_ptr<Enemy>>& allEnemies,
                std::vector<std::shared_ptr<Projectile>>& projectiles) override;
    void UpdateAnimation() override;
    void Draw() override;
    void Upgrade() override;

private:
    // 資源載入函數：根據等級自動切換路徑
    void LoadLevelAssets();

    // 儲存各等級的數值配置
    std::vector<TowerStats> m_ArcherStats;

    // --- 資源管理 ---
    std::vector<std::string> m_UpFrames;
    std::vector<std::string> m_DownFrames;

    // --- 繪製組件 ---
    std::shared_ptr<Core::Drawable> m_LeftDrawable;
    std::shared_ptr<Core::Drawable> m_RightDrawable;

    // --- 狀態管理 ---
    float m_LeftAttackStartTime = 0.0f, m_RightAttackStartTime = 0.0f;
    bool m_IsLeftAttacking = false, m_IsRightAttacking = false;
    bool m_LeftHasFired = false, m_RightHasFired = false;
    bool m_NextShooterLeft = true;
    bool m_LeftShootUp = false;
    bool m_RightShootUp = false;

    std::shared_ptr<Enemy> m_CurrentTarget;
    std::vector<std::shared_ptr<Projectile>>* m_ProjectilesRef = nullptr;
};

#endif