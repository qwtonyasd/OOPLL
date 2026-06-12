#include "SpiderMatriarch.hpp"
#include "EnemyFactory.hpp"

void SpiderMatriarch::SpawnEgg(std::vector<std::shared_ptr<Enemy>>& enemies) {
    (void)enemies; // 預留安全引用

    glm::vec2 spawnPos = m_Transform.translation;

    // 擷取女皇尚未走完的剩餘路徑，讓孵化出來的幼蛛承接
    std::vector<glm::vec2> remainingPath;
    if (m_CurrentNodeIdx < m_Path.size()) {
        remainingPath.assign(m_Path.begin() + m_CurrentNodeIdx, m_Path.end());
    } else if (!m_Path.empty()) {
        remainingPath.push_back(m_Path.back());
    }

    // 呼叫工廠接口生成蛋
    auto egg = EnemyFactory::CreateEgg(remainingPath, spawnPos);

    if (egg) {
        // 直接注入實時敵人 Vector 中（會在 App 下一個 Frame 開始被 Update）
        enemies.push_back(egg);
    }
}