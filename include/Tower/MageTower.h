#ifndef MAGE_TOWER_HPP
#define MAGE_TOWER_HPP

#include "Tower.hpp"

class MageTower : public Tower {
public:
    MageTower(glm::vec2 pos)
        : Tower(pos, "../PTSD/assets/sprites/images/MageTower/TowerLevel1/1.png", 200.0f, 1.5f, 40.0f) {}

    void Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) override {
        (void)allEnemies; (void)target;
        LOG_INFO("Mage Tower casts magic!");
        // 這裡以後會呼叫 ProjectileManager 發射 MagicBolt
    }
};

#endif