#ifndef MAGE_TOWER_H
#define MAGE_TOWER_H

#include "Tower.hpp"
#include "Tower/Projectile/Projectile.hpp" // 改用萬用投射物
#include <vector>
#include <string>

class MageTower : public Tower {
public:
    void Upgrade() override;
    MageTower(glm::vec2 pos);
    void Draw() override;
    void Attack(std::shared_ptr<Enemy> target,
                std::vector<std::shared_ptr<Enemy>>& allEnemies,
                std::vector<std::shared_ptr<Projectile>>& projectiles) override;

    void UpdateAnimation() override;

private:
    bool m_IsAttacking = false;
    bool m_HasFired = false;
    float m_AttackStartTime = 0.0f;
    std::vector<std::string> m_AttackFrames;

    std::shared_ptr<Enemy> m_CurrentTarget;
    std::vector<std::shared_ptr<Projectile>>* m_ProjectilesRef = nullptr;
};

#endif