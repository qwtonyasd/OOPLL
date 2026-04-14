#ifndef BOMB_TOWER_H
#define BOMB_TOWER_H

#include "Tower.hpp"
#include "Bomb.hpp"

class BombTower : public Tower {
public:
    BombTower(glm::vec2 pos);

    void Attack(std::shared_ptr<Enemy> target,
                std::vector<std::shared_ptr<Enemy>>& allEnemies,
                std::vector<std::shared_ptr<Projectile>>& projectiles) override;
};
#endif