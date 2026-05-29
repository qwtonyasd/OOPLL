#include "SpiderMatriarch.hpp"
#include "EnemyFactory.hpp"
#include <iostream>

void SpiderMatriarch::SpawnEgg() {
    if (m_LevelEnemyList == nullptr) return;

    std::cout << "[Boss Skill] Spider-Matriarch 產下了一顆蜘蛛卵！" << std::endl;

    auto remainingPath = m_Path; 
    if (remainingPath.empty()) return;

    // 這裡編譯器能完美看清 EnemyFactory 內的所有靜態函式
    auto egg = EnemyFactory::CreateEgg(remainingPath, GetPosition());
    if (egg) {
        m_LevelEnemyList->push_back(egg);
    }
}