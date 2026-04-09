#include "tower/BombTower.h"

void BombTower::Attack(std::shared_ptr<Enemy> target,
                      std::vector<std::shared_ptr<Enemy>>& allEnemies,
                      std::vector<std::shared_ptr<Projectile>>& projectiles) {
    // 砲彈需要 allEnemies 來在爆炸時判斷範圍內的敵人

    auto bomb = std::make_shared<Bomb>(m_Transform.translation, target->GetPosition(), m_Damage, allEnemies);
    projectiles.push_back(bomb);
}