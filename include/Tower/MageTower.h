#ifndef MAGE_TOWER_H
#define MAGE_TOWER_H

#include "Tower.hpp"
#include "Tower/Projectile/Projectile.hpp"
#include <vector>
#include <string>
#include <memory>

class MageTower : public Tower {
public:
    MageTower(glm::vec2 pos);
    virtual ~MageTower() = default;

    void Draw() override;
    void Attack(std::shared_ptr<Enemy> target,
                std::vector<std::shared_ptr<Enemy>>& allEnemies,
                std::vector<std::shared_ptr<Projectile>>& projectiles) override;
    void UpdateAnimation() override;
    void Upgrade() override;

private:
    void LoadLevelAssets();

    // 資源管理
    std::vector<std::string> m_BaseFrames;             // 1.png ~ 6.png
    std::vector<std::string> m_AttackUpFrames;         // Up 資料夾
    std::vector<std::string> m_AttackDownFrames;       // Down 資料夾
    std::string m_BulletSprite;
    std::string m_HitEffectFolder;

    // 狀態與時間管理
    bool m_IsAttacking = false;
    bool m_HasFired = false;
    bool m_IsTargetAbove = true;
    float m_AttackStartTime = 0.0f;

    std::vector<TowerStats> m_MageStats;
    std::shared_ptr<Enemy> m_CurrentTarget;
    std::vector<std::shared_ptr<Projectile>>* m_ProjectilesRef = nullptr;
};

#endif