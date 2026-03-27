#ifndef MAGE_TOWER_HPP
#define MAGE_TOWER_HPP

#include "Tower.hpp"

class MageTower : public Tower {
public:
    MageTower(glm::vec2 pos);

    void Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) override;
};

#endif