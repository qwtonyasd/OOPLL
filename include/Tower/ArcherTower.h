#ifndef ARCHER_TOWER_H
#define ARCHER_TOWER_H

#include "Tower.hpp"
#include "Arrow.hpp"

class ArcherTower : public Tower {
public:
    ArcherTower(glm::vec2 pos);

    void Attack(std::shared_ptr<Enemy> target,
                std::vector<std::shared_ptr<Enemy>>& allEnemies,
                std::vector<std::shared_ptr<Projectile>>& projectiles) override;
};
#endif