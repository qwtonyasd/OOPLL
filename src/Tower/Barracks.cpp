#include "tower/Barracks.h"
#include "Util/Time.hpp"
#include "MapData.hpp"

Barracks::Barracks(glm::vec2 pos, const std::vector<glm::vec2>& route)
    : Tower(pos, "../PTSD/assets/sprites/images/BarracksTower/TowerLevel1/1.png",
            120.0f, 10.0f, 0.0f, 70, Enemy::DamageType::PHYSICAL), m_Route(route) {

    for (int i = 0; i < m_MaxSoldiers; ++i) {
        SpawnSoldier(i);
    }
    // 這裡的 ZIndex 建議設低一點，讓小兵在塔前面
    m_ZIndex = 40.0f;
}

void Barracks::Update(std::vector<std::shared_ptr<Enemy>>& enemies,
                      [[maybe_unused]] std::vector<std::shared_ptr<Projectile>>& projectiles) {
    float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;

    // 1. 小兵復活邏輯
    for (int i = 0; i < m_MaxSoldiers; ++i) {
        auto& slot = m_Slots[i];
        if (slot.soldier) {
            slot.soldier->Update(enemies);
            if (slot.soldier->GetHP() <= 0 && slot.soldier->IsDeadAnimationFinished()) {
                slot.soldier->ReleaseEnemy();
                slot.soldier = nullptr;
                slot.isWaitingForRespawn = true;
                slot.respawnTimer = 0.0f;
            }
        } else if (slot.isWaitingForRespawn) {
            slot.respawnTimer += dt;
            if (slot.respawnTimer >= 10.0f) {
                SpawnSoldier(i);
                slot.isWaitingForRespawn = false;
            }
        }
    }

    // 2. 攔截敵人邏輯
    glm::vec2 rallyPos = MapData::GetBaseRallyPoint(m_Transform.translation);
    for (auto& enemy : enemies) {
        if (!enemy || enemy->GetHP() <= 0) continue;
        if (glm::distance(enemy->GetPosition(), rallyPos) < 120.0f) {
            for (int i = 0; i < m_MaxSoldiers; ++i) {
                if (m_Slots[i].soldier && m_Slots[i].soldier->IsAvailable()) {
                    m_Slots[i].soldier->EngageTarget(enemy);
                    // 兵營不需要 break 敵人，因為一個敵人可以被多個小兵圍毆
                    // 但一個小兵只能擋一個敵人，這部分邏輯應在 Soldier::EngageTarget 處理
                }
            }
        }
    }
}

void Barracks::SpawnSoldier(int slotIndex) {
    glm::vec2 baseRallyPoint = MapData::GetBaseRallyPoint(m_Transform.translation);

    // 從塔門口出發 (微調座標讓它看起來像從門出來)
    glm::vec2 spawnPos = m_Transform.translation + glm::vec2(0, -10.0f);

    glm::vec2 formationOffset;
    if (slotIndex == 0)      formationOffset = {0, 0};
    else if (slotIndex == 1) formationOffset = {-20, -15};
    else                     formationOffset = {20, -15};

    m_Slots[slotIndex].soldier = std::make_shared<Soldier>(
        spawnPos,
        baseRallyPoint + formationOffset,
        1.5f,  // 移速
        50.0f  // 血量
    );
}

void Barracks::UpdateAnimation() {
    // 兵營目前沒有攻擊動畫，暫不實作
}

void Barracks::Draw() {
    // 1. 畫範圍圈與選單 (父類別處理)
    Tower::Draw();

    // 2. 畫塔本體 (如果你在建構子已經 SetDrawable，這裡只要呼叫 GameObject 的 Draw)
    // 注意：Tower::Draw 已經包含了基礎渲染，如果你的塔圖層怪怪的，
    // 請檢查 Tower.cpp 的 Draw 實作。

    // 3. 畫所有生存的小兵
    for (int i = 0; i < m_MaxSoldiers; ++i) {
        if (m_Slots[i].soldier) {
            m_Slots[i].soldier->Draw();
        }
    }
}

void Barracks::Attack([[maybe_unused]] std::shared_ptr<Enemy> t,
                      [[maybe_unused]] std::vector<std::shared_ptr<Enemy>>& e,
                      [[maybe_unused]] std::vector<std::shared_ptr<Projectile>>& p) {
    // 兵營不使用 Attack 觸發子彈
}

void Barracks::Upgrade() {
    // 留空即可，目的是滿足編譯器的要求
}