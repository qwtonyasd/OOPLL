#ifndef ARCHER_TOWER_H
#define ARCHER_TOWER_H

#include "Tower.hpp"
#include "Arrow.hpp" // 假設你有 Arrow 類別繼承自 Projectile

class ArcherTower : public Tower {
public:
    ArcherTower(glm::vec2 pos)
        : Tower(pos, "../PTSD/assets/sprites/images/ArcherTower/TowerLevel1/1.png", 250.0f, 0.8f, 10.0f, 70) {}

    void Attack(std::shared_ptr<Enemy> target,
                std::vector<std::shared_ptr<Enemy>>& allEnemies,
                std::vector<std::shared_ptr<Projectile>>& projectiles) override;
};
#endif