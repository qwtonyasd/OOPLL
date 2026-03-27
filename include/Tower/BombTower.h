#ifndef BOMB_TOWER_HPP
#define BOMB_TOWER_HPP

#include "Tower.hpp"

class BombTower : public Tower {
public:
    BombTower(glm::vec2 pos)
        : Tower(pos, "../PTSD/assets/sprites/images/BombTower/TowerLevel1/1.png", 180.0f, 2.5f, 50.0f) {}

    void Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) override {
        LOG_INFO("Bomb Tower explosion!");
        glm::vec2 explosionPos = target->GetTransform().translation;
        float explosionRadius = 100.0f;
        for (auto& enemy : allEnemies) {
            if (glm::distance(explosionPos, enemy->GetTransform().translation) <= explosionRadius) {
                // enemy->DecreaseHP(m_Damage); 
            }
        }
    }
};

#endif