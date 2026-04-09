#include "tower/Barracks.h"
#include "Util/Time.hpp"
#include <algorithm>

Barracks::Barracks(glm::vec2 pos, const std::vector<glm::vec2>& route)
    : Tower(pos, "../PTSD/assets/sprites/images/BarracksTower/TowerLevel1/1.png", 0.0f, 10.0f, 0.0f, 70),
      m_Route(route) {

    m_MaxSoldiers = 3;
    m_SpawnTimer = 0.0f;

    // 初始生成三名士兵
    for (int i = 0; i < m_MaxSoldiers; ++i) {
        SpawnSoldier();
    }
}

void Barracks::Update(std::vector<std::shared_ptr<Enemy>>& enemies) {
    float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;

    // 1. 補兵邏輯
    if (m_Soldiers.size() < (size_t)m_MaxSoldiers) {
        m_SpawnTimer += dt;
        if (m_SpawnTimer >= 10.0f) {
            SpawnSoldier();
            m_SpawnTimer = 0.0f;
        }
    }

    // 2. 更新士兵 AI 與戰鬥
    for (auto& soldier : m_Soldiers) {
        soldier->Update(enemies);

        // 額外保險：如果士兵正在戰鬥，且目標敵人的 HP 歸零，強制敵人進入死亡狀態
        // 這能解決某些情況下 Enemy 自己沒偵測到死亡的問題
        // 注意：這裡假設 Soldier 類別有一個 GetTargetEnemy() 的方法
        // 如果沒有，則由 Soldier 內部的 PerformAttack 處理扣血
    }

    // 3. 管理士兵本身的生死移除
    for (auto it = m_Soldiers.begin(); it != m_Soldiers.end(); ) {
        if ((*it)->GetHP() <= 0) {
            if ((*it)->IsDeadAnimationFinished()) {
                (*it)->ReleaseEnemy();
                it = m_Soldiers.erase(it);
                continue;
            }
        }
        ++it;
    }
}

void Barracks::SpawnSoldier() {
    glm::vec2 relativePos;
    // 這裡使用目前陣列大小來決定位置，避免重疊
    size_t id = m_Soldiers.size();

    if (id == 0)      relativePos = glm::vec2(0.0f, -45.0f);
    else if (id == 1) relativePos = glm::vec2(-25.0f, -70.0f);
    else              relativePos = glm::vec2(25.0f, -70.0f);

    glm::vec2 rallyPoint = m_Transform.translation + relativePos;

    auto newSoldier = std::make_shared<Soldier>(
        m_Transform.translation,
        rallyPoint,
        1.3f,
        100.0f
    );

    m_Soldiers.push_back(newSoldier);
}

void Barracks::Draw() {
    Tower::Draw();
    for (auto& soldier : m_Soldiers) {
        soldier->Draw();
    }
}

void Barracks::Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) {
    // 兵營不使用此函式，由士兵自行在 Update 中尋找目標
    (void)target;
    (void)allEnemies;
}