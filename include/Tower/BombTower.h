#ifndef BOMB_TOWER_H
#define BOMB_TOWER_H

#include "Tower.hpp"
#include "Bomb.hpp" // 假設你有 Bomb 類別繼承自 Projectile

class BombTower : public Tower {
public:
    BombTower(glm::vec2 pos)
        : Tower(pos, "../PTSD/assets/sprites/images/BombTower/TowerLevel1/1.png", 150.0f, 2.5f, 40.0f, 125) {}

    void Attack(std::shared_ptr<Enemy> target,
                std::vector<std::shared_ptr<Enemy>>& allEnemies,
                std::vector<std::shared_ptr<Projectile>>& projectiles) override;
};
#endif