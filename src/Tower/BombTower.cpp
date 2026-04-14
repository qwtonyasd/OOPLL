#include "tower/BombTower.h"

BombTower::BombTower(glm::vec2 pos)
    : Tower(pos, "../PTSD/assets/sprites/images/BombTower/TowerLevel1/1.png",
            150.0f, 2.5f, 40.0f, 125, Enemy::DamageType::PHYSICAL) {}

void BombTower::Attack(std::shared_ptr<Enemy> target,
                      std::vector<std::shared_ptr<Enemy>>& allEnemies,
                      std::vector<std::shared_ptr<Projectile>>& projectiles) {
    // 傳入 m_DamageType (PHYSICAL)
    auto bomb = std::make_shared<Bomb>(m_Transform.translation, target->GetPosition(), m_Damage, allEnemies, m_DamageType);
    projectiles.push_back(bomb);
}