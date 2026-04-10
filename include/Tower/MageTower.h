#ifndef MAGE_TOWER_H
#define MAGE_TOWER_H

#include "Tower.hpp"
#include "MagicBolt.hpp"

class MageTower : public Tower {
public:
    MageTower(glm::vec2 pos)
        : Tower(pos, "../PTSD/assets/sprites/images/MageTower/TowerLevel1/1.png", 280.0f, 1.5f, 35.0f, 100) {}

    void Attack(std::shared_ptr<Enemy> target,
                std::vector<std::shared_ptr<Enemy>>& allEnemies,
                std::vector<std::shared_ptr<Projectile>>& projectiles) override;
};

#endif