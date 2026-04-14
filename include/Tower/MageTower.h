#ifndef MAGE_TOWER_H
#define MAGE_TOWER_H

#include "Tower.hpp"
#include "MagicBolt.hpp"

class MageTower : public Tower {
public:
    MageTower(glm::vec2 pos);

    void Attack(std::shared_ptr<Enemy> target,
                std::vector<std::shared_ptr<Enemy>>& allEnemies,
                std::vector<std::shared_ptr<Projectile>>& projectiles) override;
};
#endif