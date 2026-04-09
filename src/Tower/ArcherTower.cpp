#include "tower/ArcherTower.h"

void ArcherTower::Attack(std::shared_ptr<Enemy> target,
                        std::vector<std::shared_ptr<Enemy>>& allEnemies,
                        std::vector<std::shared_ptr<Projectile>>& projectiles) {
    (void)allEnemies; // 弓箭塔是單體攻擊，暫不需要全體清單

    // 產生箭矢並加入容器
    auto arrow = std::make_shared<Arrow>(m_Transform.translation, target, m_Damage);
    projectiles.push_back(arrow);
}