#include "Egg.hpp"
#include "EnemyFactory.hpp"

std::vector<std::shared_ptr<Enemy>> Egg::s_SpawnQueue = {};

void Egg::Hatch(std::vector<std::shared_ptr<Enemy>>& enemies) {
    (void)enemies;

    std::vector<glm::vec2> remainingPath;

    if (m_CurrentNodeIdx < m_Path.size()) {
        remainingPath.assign(m_Path.begin() + m_CurrentNodeIdx, m_Path.end());
    }
    if (remainingPath.empty() && !m_Path.empty()) {
        remainingPath.push_back(m_Path.back());
    }

    // 透過工廠建立常規小蜘蛛
    auto spiderling = EnemyFactory::Create(Enemy::Type::SPIDERLING, remainingPath);

    if (spiderling) {
        // 直接利用繼承權限修改 protected 的位置屬性，安全又乾淨
        spiderling->m_Transform = this->m_Transform;

        // 塞進暫存佇列，交給 App.cpp 安全釋放
        Egg::s_SpawnQueue.push_back(spiderling);
    }
}