#include "Tower/ArcherTower.h"
#include "Util/Logger.hpp"

ArcherTower::ArcherTower(glm::vec2 pos)
    : Tower(pos, "../PTSD/assets/sprites/images/ArcherTower/TowerLevel1/1.png", 250.0f, 0.7f, 15.0f) {
    LOG_INFO("Archer Tower created at ({}, {})", pos.x, pos.y);
}

void ArcherTower::Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) {
    (void)allEnemies;
    (void)target;
    LOG_INFO("Archer Tower fires an arrow!");
    // 未來實作：m_Manager->SpawnArrow(m_Transform.translation, target, m_Damage);
}