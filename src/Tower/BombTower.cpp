#include "tower/BombTower.h"
#include "Util/Logger.hpp"

BombTower::BombTower(glm::vec2 pos)
    // 最後補上 125
    : Tower(pos, "../PTSD/assets/sprites/images/BombTower/TowerLevel1/1.png", 180.0f, 2.5f, 50.0f, 125) {}

void BombTower::Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) {
    LOG_INFO("Bomb Tower explosion!");
    glm::vec2 explosionPos = target->GetTransform().translation;
    float explosionRadius = 100.0f;

    for (auto& enemy : allEnemies) {
        if (glm::distance(explosionPos, enemy->GetTransform().translation) <= explosionRadius) {
            enemy->TakeDamage(m_Damage); // 這裡可以直接取消註解並改用 TakeDamage
        }
    }
}