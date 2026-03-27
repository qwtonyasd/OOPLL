#ifndef ARCHER_TOWER_HPP
#define ARCHER_TOWER_HPP

#include "Tower.hpp"

class ArcherTower : public Tower {
public:
    ArcherTower(glm::vec2 pos);

    void Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) override;
};

#endif