#include "tower/MageTower.h"
#include "Util/Logger.hpp"

MageTower::MageTower(glm::vec2 pos)
    // 最後補上 100
    : Tower(pos, "../PTSD/assets/sprites/images/MageTower/TowerLevel1/1.png", 200.0f, 1.5f, 40.0f, 100) {}

void MageTower::Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) {
    (void)allEnemies;
    (void)target;
    LOG_INFO("Mage Tower casts magic bolt!");
}