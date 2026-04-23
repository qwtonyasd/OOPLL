#include "tower/Barracks.h"
#include "Util/Time.hpp"
#include "MapData.hpp"

Barracks::Barracks(glm::vec2 pos, const std::vector<glm::vec2>& [[maybe_unused]] route)
    : Tower(pos, "../PTSD/assets/sprites/images/BarracksTower/TowerLevel1/1.png",
            120.0f, 10.0f, 0.0f, 70, Enemy::DamageType::PHYSICAL), m_Route(route) {

    // 初始化時將 3 個席位填滿小兵
    for (int i = 0; i < m_MaxSoldiers; ++i) {
        SpawnSoldier(i); // 傳遞 slotIndex
    }
    m_VisualOffset = 10.0f;
}

void Barracks::Update(std::vector<std::shared_ptr<Enemy>>& enemies,
                      [[maybe_unused]] std::vector<std::shared_ptr<Projectile>>& projectiles) {
    float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;

    // 1. 小兵狀態管理與復活
    for (int i = 0; i < m_MaxSoldiers; ++i) {
        auto& slot = m_Slots[i]; // 使用 Slot 結構而非 vector

        if (slot.soldier) {
            slot.soldier->Update(enemies);

            // 檢查死亡且動畫結束
            if (slot.soldier->GetHP() <= 0 && slot.soldier->IsDeadAnimationFinished()) {
                slot.soldier->ReleaseEnemy();
                slot.soldier = nullptr;        // 席位清空
                slot.isWaitingForRespawn = true; // 開始計時
                slot.respawnTimer = 0.0f;
            }
        }
        else if (slot.isWaitingForRespawn) {
            slot.respawnTimer += dt;
            if (slot.respawnTimer >= 10.0f) { // 獨立復活時間 10 秒
                SpawnSoldier(i);
                slot.isWaitingForRespawn = false;
            }
        }
    }

    // 2. 指揮官派發任務 (協同作戰)
    glm::vec2 rallyPos = MapData::GetBaseRallyPoint(m_Transform.translation);
    for (auto& enemy : enemies) {
        if (!enemy || enemy->GetHP() <= 0) continue;

        // 如果敵人進入守備範圍
        if (glm::distance(enemy->GetPosition(), rallyPos) < 120.0f) {
            bool assigned = false;
            for (int i = 0; i < m_MaxSoldiers; ++i) {
                if (m_Slots[i].soldier && m_Slots[i].soldier->IsAvailable()) {
                    m_Slots[i].soldier->EngageTarget(enemy);
                    assigned = true;
                }
            }
            if (assigned) break; // 優先處理最前面的怪
        }
    }
}

void Barracks::SpawnSoldier(int slotIndex) {
    glm::vec2 baseRallyPoint = MapData::GetBaseRallyPoint(m_Transform.translation);
    glm::vec2 originalPos = m_Transform.translation;

    // --- 調整這裡的數值 (例如 +10 或 +20)，直到位置看起來正確 ---
    m_Transform.translation.y += 3.0f;
    // 陣型偏移
    glm::vec2 formationOffset;
    if (slotIndex == 0)      formationOffset = {0, 0};
    else if (slotIndex == 1) formationOffset = {-20, -15};
    else                     formationOffset = {20, -15};

    glm::vec2 finalTarget = baseRallyPoint + formationOffset;

    // 建立新小兵
    m_Slots[slotIndex].soldier = std::make_shared<Soldier>(
        m_Transform.translation,
        finalTarget,
        1.5f,
        50.0f
    );
}

void Barracks::Draw() {
    Tower::Draw(); // 畫塔本身
    if (m_Visible && m_Drawable) {
        auto data = Util::ConvertToUniformBufferData(
            m_Transform,
            m_Drawable->GetSize(),
            m_ZIndex
        );
        m_Drawable->Draw(data);
    }
    for (int i = 0; i < m_MaxSoldiers; ++i) {
        if (m_Slots[i].soldier) {
            m_Slots[i].soldier->Draw(); // 畫每個席位的小兵
        }
    }
}

void Barracks::Attack([[maybe_unused]] std::shared_ptr<Enemy> t,
                      [[maybe_unused]] std::vector<std::shared_ptr<Enemy>>& e,
                      [[maybe_unused]] std::vector<std::shared_ptr<Projectile>>& p) {
    // 兵營不主動攻擊，由小兵代勞
}
