#include "tower/Barracks.h"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"

Barracks::Barracks(glm::vec2 pos, const std::vector<glm::vec2>& route)
    : Tower(pos, "../PTSD/assets/sprites/images/BarracksTower/TowerLevel1/1.png", 0.0f, 10.0f, 0.0f, 70),
      m_Route(route) {

    m_SpawnTimer = 0.0f;
    // 初始化時先生產三隻小兵，不要讓玩家等
    SpawnSoldier();
    SpawnSoldier();
    SpawnSoldier();

    LOG_INFO("Barracks created at ({}, {})", pos.x, pos.y);
}

void Barracks::Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) {
    // 兵營不需要傳統的遠程攻擊，邏輯已移至 Update 中的士兵行為
    (void)target;
    (void)allEnemies;
}

void Barracks::Update(std::vector<std::shared_ptr<Enemy>>& enemies) {
    float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;
    m_SpawnTimer += dt;

    // 每 10 秒檢查一次是否需要補兵
    if (m_SpawnTimer >= 10.0f) {
        if (m_Soldiers.size() < 3) {
            SpawnSoldier();
        }
        m_SpawnTimer = 0.0f;
    }

    // 更新所有士兵的行為邏輯
    for (auto it = m_Soldiers.begin(); it != m_Soldiers.end(); ) {
        // 呼叫我們在 Soldier.hpp 定義的 UpdateLogic 並傳入敵人列表
        (*it)->UpdateLogic(enemies);

        // 判定士兵是否完全死亡（血量 <= 0 且 死亡動畫播完）
        if ((*it)->GetHP() <= 0 && (*it)->IsDeadAnimationFinished()) {
            it = m_Soldiers.erase(it);
        } else {
            ++it;
        }
    }
}

void Barracks::SpawnSoldier() {
    if (m_Soldiers.size() < 3) {
        // 設定集結點：在兵營位置下方 40 像素處
        glm::vec2 rallyPoint = m_Transform.translation + glm::vec2(0.0f, -40.0f);

        // 創建士兵：傳入 1. 出生點, 2. 集結點, 3. 速度, 4. 血量
        // 內部的圖片路徑載入現在已寫在 Soldier 的建構子內
        auto s = std::make_shared<Soldier>(
            m_Transform.translation,
            rallyPoint,
            1.5f,
            100.0f
        );

        m_Soldiers.push_back(s);
        LOG_DEBUG("A new soldier has been dispatched from Barracks.");
    }
}

void Barracks::Draw() {
    // 1. 畫出兵營建築物本身
    Tower::Draw();

    // 2. 畫出兵營所屬的所有士兵
    for (auto& s : m_Soldiers) {
        s->Draw();
    }
}