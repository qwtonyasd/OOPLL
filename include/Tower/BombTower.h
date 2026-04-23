#ifndef BOMB_TOWER_H
#define BOMB_TOWER_H

#include "Tower.hpp"
#include "Bomb.hpp"
#include <vector>
#include <string>
#include <memory> // 確保有包含這個

class BombTower : public Tower {
public:
    BombTower(glm::vec2 pos);
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

    // --- 核心修正：加入這幾行 ---
    std::shared_ptr<Enemy> m_CurrentTarget; // 讓類別能記住目標
    std::vector<std::shared_ptr<Enemy>>* m_AllEnemiesRef = nullptr;
    std::vector<std::shared_ptr<Projectile>>* m_ProjectilesRef = nullptr;
};

#endif