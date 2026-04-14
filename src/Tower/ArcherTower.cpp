#include "tower/ArcherTower.h"

ArcherTower::ArcherTower(glm::vec2 pos)
    : Tower(pos, "../PTSD/assets/sprites/images/ArcherTower/TowerLevel1/1.png",
            250.0f, 0.8f, 10.0f, 70, Enemy::DamageType::PHYSICAL) {}

void ArcherTower::Attack(std::shared_ptr<Enemy> target,
                        std::vector<std::shared_ptr<Enemy>>& allEnemies,
                        std::vector<std::shared_ptr<Projectile>>& projectiles) {
    (void)allEnemies;
    // 傳入 m_DamageType (PHYSICAL)
    auto arrow = std::make_shared<Arrow>(m_Transform.translation, target, m_Damage, m_DamageType);
    projectiles.push_back(arrow);
}