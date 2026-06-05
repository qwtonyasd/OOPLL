#include "SpiderMatriarch.hpp"
#include "EnemyFactory.hpp" // 在 .cpp 引入工廠，完美解開標頭檔循環引用

void SpiderMatriarch::SpawnEgg(std::vector<std::shared_ptr<Enemy>>& enemies) {
    // 獲取女皇當前的位置
    glm::vec2 spawnPos = m_Transform.translation;

    // 擷取女皇尚未走完的剩餘路徑，讓孵化出來的幼蛛知道接下來該往哪走
    std::vector<glm::vec2> remainingPath;
    if (m_CurrentNodeIdx < m_Path.size()) {
        remainingPath.assign(m_Path.begin() + m_CurrentNodeIdx, m_Path.end());
    } else if (!m_Path.empty()) {
        remainingPath.push_back(m_Path.back());
    }

    // 🎯 呼叫工廠接口生成蛋
    auto egg = EnemyFactory::CreateEgg(remainingPath, spawnPos);

    if (egg) {
        // 🎯 將新生成的蛋動態注入實時的敵人 Vector 中
        enemies.push_back(egg);
    }
}