#ifndef BOMB_TOWER_HPP
#define BOMB_TOWER_HPP

#include "Tower.hpp"

class BombTower : public Tower {
public:
    BombTower(glm::vec2 pos);

    void Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) override;
};

#endif