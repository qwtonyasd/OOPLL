#ifndef ARCHER_TOWER_HPP
#define ARCHER_TOWER_HPP

#include "Tower.hpp"

class ArcherTower : public Tower {
public:
    ArcherTower(glm::vec2 pos)
        : Tower(pos, "../PTSD/assets/sprites/images/ArcherTower/TowerLevel1/1.png", 250.0f, 0.7f, 15.0f) {}

    void Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) override {
        (void)allEnemies; (void)target;
        LOG_INFO("Archer Tower fires!");
        // 這裡以後會呼叫 ProjectileManager 發射 Arrow
    }
};

#endif