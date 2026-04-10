#include "tower/MageTower.h"
#include "MagicBolt.hpp"
#include "Util/Logger.hpp"

void MageTower::Attack(std::shared_ptr<Enemy> target,
                      std::vector<std::shared_ptr<Enemy>>& enemies,
                      std::vector<std::shared_ptr<Projectile>>& projectiles) {
    if (target) {
        // --- 暴力測試：直接使用塔的「當前實際座標」 ---
        // 不要手動輸入 (640, 360)，直接拿 m_Transform.translation
        glm::vec2 spawnPos = this->m_Transform.translation;

        auto bolt = std::make_shared<MagicBolt>(spawnPos, target, m_Damage);

        // 強制確保子彈這時候是活著的
        bolt->SetZIndex(100.0f);

        projectiles.push_back(bolt);

        LOG_INFO("MageBolt Created at ({}, {})", spawnPos.x, spawnPos.y);
    }
}