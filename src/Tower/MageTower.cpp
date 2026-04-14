#include "tower/MageTower.h"

MageTower::MageTower(glm::vec2 pos)
    : Tower(pos, "../PTSD/assets/sprites/images/MageTower/TowerLevel1/1.png",
            280.0f, 1.5f, 35.0f, 100, Enemy::DamageType::MAGIC) {}

void MageTower::Attack(std::shared_ptr<Enemy> target,
                      std::vector<std::shared_ptr<Enemy>>& enemies,
                      std::vector<std::shared_ptr<Projectile>>& projectiles) {
    (void)enemies;
    if (target) {
        // 傳入 m_DamageType (MAGIC)
        auto bolt = std::make_shared<MagicBolt>(m_Transform.translation, target, m_Damage, m_DamageType);
        projectiles.push_back(bolt);
    }
}