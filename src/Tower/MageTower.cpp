#include "tower/MageTower.h"

void MageTower::Attack(std::shared_ptr<Enemy> target,
                      std::vector<std::shared_ptr<Enemy>>& allEnemies,
                      std::vector<std::shared_ptr<Projectile>>& projectiles) {
    (void)allEnemies; // 暫時沒用到，避免警告

    // 這裡實作法師塔射擊邏輯（產生法術球之類的）
    // 例如：
    // auto magicBolt = std::make_shared<MagicBolt>(m_Transform.translation, target, m_Damage);
    // projectiles.push_back(magicBolt);
    (void)target; (void)projectiles;
}