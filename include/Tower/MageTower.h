#ifndef MAGE_TOWER_H
#define MAGE_TOWER_H

#include "Tower.hpp"
#include "MagicBolt.hpp"
#include <vector>
#include <string>

class MageTower : public Tower {
public:
    MageTower(glm::vec2 pos);

    void Attack(std::shared_ptr<Enemy> target,
                std::vector<std::shared_ptr<Enemy>>& allEnemies,
                std::vector<std::shared_ptr<Projectile>>& projectiles) override;

    void UpdateAnimation() override; // 記得覆寫這個函式

private:
    // --- 動畫相關 ---
    bool m_IsAttacking = false;
    bool m_HasFired = false;
    float m_AttackStartTime = 0.0f;
    std::vector<std::string> m_AttackFrames;

    // --- 暫存發射所需資料 ---
    std::shared_ptr<Enemy> m_CurrentTarget;
    std::vector<std::shared_ptr<Projectile>>* m_ProjectilesRef = nullptr;
};

#endif