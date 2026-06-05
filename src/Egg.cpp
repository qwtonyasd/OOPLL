#include "Egg.hpp"
#include "EnemyFactory.hpp"

std::vector<std::shared_ptr<Enemy>> Egg::s_SpawnQueue = {};

void Egg::Hatch(std::vector<std::shared_ptr<Enemy>>& enemies) {
    (void)enemies;

    // 🎯 核心修正：不管蛋在原地名義上走到了哪裡，小蜘蛛一出生，
    // 就必須100%精準承接從「蛋目前所在的這格路線節點」開始之後的所有完整剩餘路徑。
    std::vector<glm::vec2> remainingPath;

    // 如果蛋的基底類別路徑計數器還在安全範圍，我們強制從當前精確節點開始切
    if (m_CurrentNodeIdx < m_Path.size()) {
        remainingPath.assign(m_Path.begin() + m_CurrentNodeIdx, m_Path.end());
    }
    // 安全防護：萬一真的不小心走到底了，就給牠地圖最後一個點
    if (remainingPath.empty() && !m_Path.empty()) {
        remainingPath.push_back(m_Path.back());
    }

    // 透過工廠建立常規小蜘蛛
    auto spiderling = EnemyFactory::Create(Enemy::Type::SPIDERLING, remainingPath);

    if (spiderling) {
        // 合法轉型以複製 protected 的 m_Transform
        auto eggCast = std::static_pointer_cast<Egg>(spiderling);
        eggCast->m_Transform = this->m_Transform;

        // 塞進暫存佇列，交給 App.cpp 稍後釋放
        Egg::s_SpawnQueue.push_back(spiderling);
    }
}