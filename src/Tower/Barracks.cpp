#include "tower/Barracks.h"
#include "Util/Time.hpp"
#include "MapData.hpp"

Barracks::Barracks(glm::vec2 pos, const std::vector<glm::vec2>& route)
    : Tower(pos, "../PTSD/assets/sprites/images/BarracksTower/TowerLevel1/1.png",
            120.0f, 10.0f, 0.0f, 70, Enemy::DamageType::PHYSICAL), m_Route(route) {

    // 1. 定義數據表：{範圍, 復活冷卻, 傷害(小兵), 升級費用, 基座圖路徑}
    m_BarracksStats = {
        {120.0f, 10.0f, 3.0f, 110, "../PTSD/assets/sprites/images/BarracksTower/TowerLevel1/1.png"},
        {140.0f, 8.0f,  6.0f, 160, "../PTSD/assets/sprites/images/BarracksTower/TowerLevel2/1.png"},
        {160.0f, 7.0f,  10.0f, 0,  "../PTSD/assets/sprites/images/BarracksTower/TowerLevel3/1.png"}
    };

    // 2. 套用 Level 1 數據：這會將 m_Cost 初始化為 110 (升級價)
    ApplyBaseStats(m_BarracksStats[0]);
    m_SoldierHP = 50.0f; // 初始血量
    m_SoldierDamage = m_BarracksStats[0].damage;

    // 3. 生成初始小兵
    for (int i = 0; i < m_MaxSoldiers; ++i) {
        SpawnSoldier(i);
    }

    m_ZIndex = 40.0f;
    UpdateCostText();
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
            // 復活冷卻時間隨等級縮短 (m_Cooldown)
            if (slot.respawnTimer >= m_Cooldown) {
                SpawnSoldier(i);
                slot.isWaitingForRespawn = false;
            }
        }
    }

    // 2. 攔截敵人邏輯
    glm::vec2 rallyPos = MapData::GetBaseRallyPoint(m_Transform.translation);
    for (auto& enemy : enemies) {
        if (!enemy || enemy->GetHP() <= 0) continue;
        if (glm::distance(enemy->GetPosition(), rallyPos) < m_Range) {
            for (int i = 0; i < m_MaxSoldiers; ++i) {
                if (m_Slots[i].soldier && m_Slots[i].soldier->IsAvailable()) {
                    m_Slots[i].soldier->EngageTarget(enemy);
                }
            }
        }
    }
}

void Barracks::SpawnSoldier(int slotIndex) {
    glm::vec2 baseRallyPoint = MapData::GetBaseRallyPoint(m_Transform.translation);
    glm::vec2 spawnPos = m_Transform.translation + glm::vec2(0, -10.0f);

    glm::vec2 formationOffset;
    if (slotIndex == 0)      formationOffset = {0, 0};
    else if (slotIndex == 1) formationOffset = {-20, -15};
    else                     formationOffset = {20, -15};

    // 建立小兵時傳入當前等級的血量與傷害
    m_Slots[slotIndex].soldier = std::make_shared<Soldier>(
        spawnPos,
        baseRallyPoint + formationOffset,
        1.5f,        // 移速
        m_SoldierHP  // 血量
    );
}

void Barracks::Upgrade() {
    if (m_Level < m_MaxLevel) {
        m_Level++;

        // 套用下一級數據 (金額、範圍、冷卻)
        ApplyBaseStats(m_BarracksStats[m_Level - 1]);

        // 提升小兵基礎能力值
        m_SoldierHP += 20.0f;
        m_SoldierDamage = m_BarracksStats[m_Level - 1].damage;

        // 升級時立即治癒或重生存活的小兵（可選，增加玩家升級感）
        for (int i = 0; i < m_MaxSoldiers; ++i) {
            SpawnSoldier(i);
            m_Slots[i].isWaitingForRespawn = false;
        }

        LOG_INFO("Barracks Upgraded to Level {}!", m_Level);
    }
}

void Barracks::Draw() {
    // 1. 畫父類別的 UI (範圍圈、圓環、110數字、升級/出售圖示)
    Tower::Draw();

    // 2. 畫塔建築物本體 (確保它被畫出來)
    if (m_Visible && m_Drawable) {
        // 使用與其他塔一致的 ZIndex (例如 15.0f)，確保它在範圍圈之上
        auto data = Util::ConvertToUniformBufferData(m_Transform, m_Drawable->GetSize(), 15.0f);
        m_Drawable->Draw(data);
    }

    // 3. 畫小兵 (確保小兵 ZIndex 高於塔或低於塔，依你喜好)
    for (int i = 0; i < m_MaxSoldiers; ++i) {
        if (m_Slots[i].soldier) {
            m_Slots[i].soldier->Draw();
        }
    }
}

void Barracks::UpdateAnimation() { }

void Barracks::Attack(std::shared_ptr<Enemy>, std::vector<std::shared_ptr<Enemy>>&, std::vector<std::shared_ptr<Projectile>>&) { }