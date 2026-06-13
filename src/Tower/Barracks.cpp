#include "tower/Barracks.h"
#include "Util/Time.hpp"
#include "MapData.hpp"

Barracks::Barracks(glm::vec2 pos, const std::vector<glm::vec2>& route)
    : Tower(pos, "../PTSD/assets/sprites/images/BarracksTower/TowerLevel1/1.png",
            120.0f, 10.0f, 0.0f, 70, Enemy::DamageType::PHYSICAL), m_Route(route) {

    // 1. 定義數據表：{範圍, 復活冷卻, 傷害(小兵), 升級費用, 基座圖路徑}
    m_BarracksStats = {
        {120.0f, 10.0f, 2.0f, 110, "../PTSD/assets/sprites/images/BarracksTower/TowerLevel1/1.png"},
        {140.0f, 8.0f,  3.0f, 160, "../PTSD/assets/sprites/images/BarracksTower/TowerLevel2/1.png"},
        {160.0f, 7.0f,  5.0f, 0,  "../PTSD/assets/sprites/images/BarracksTower/TowerLevel3/1.png"}
    };

    // 2. 套用 Level 1 數據：這會將 m_Cost 初始化為 110 (升級價)
    ApplyBaseStats(m_BarracksStats[0]);

    m_SoldierHP = 50.0f; // 初始血量
    m_SoldierDamage = m_BarracksStats[0].damage;

    // 🆕 【增加這段】：初始建造時套用天賦加成（兵營固定對應 Index 1）
    auto& gd = GameData::GetInstance();
    int talentLv = gd.talentLevels[1];
    m_SoldierHP *= gd.GetBarrackHP(talentLv);
    m_SoldierDamage *= gd.GetBarrackDamage(talentLv);

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
    if (dt > 0.1f) dt = 0.016f;

    // 1. 小兵更新與死掉復活邏輯
    for (int i = 0; i < m_MaxSoldiers; ++i) {
        auto& slot = m_Slots[i];
        if (slot.soldier) {
            slot.soldier->Update(enemies, dt); // 傳入統一的 dt
            if (slot.soldier->GetHP() <= 0 && slot.soldier->IsDeadAnimationFinished()) {
                slot.soldier->ReleaseEnemy();
                slot.soldier = nullptr;
                slot.isWaitingForRespawn = true;
                slot.respawnTimer = 0.0f;
            }
        } else if (slot.isWaitingForRespawn) {
            slot.respawnTimer += dt;
            if (slot.respawnTimer >= m_Cooldown) {
                SpawnSoldier(i);
                slot.isWaitingForRespawn = false;
            }
        }
    }

    // 2. 僅下達攔截命令
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
    //glm::vec2 spawnPos = m_Transform.translation + glm::vec2(0, -10.0f);

    glm::vec2 formationOffset = (slotIndex == 0) ? glm::vec2(0, 0) :
                                (slotIndex == 1) ? glm::vec2(-20, -15) : glm::vec2(20, -15);
    glm::vec2 spawnPos = baseRallyPoint + formationOffset;
    // 1. 在兵營打包給步兵的資源
    Soldier::SoldierConfig genericInfantry;

    // 根據目前的等級（m_Level）動態改變小兵的造型讀取資料夾與動畫影格
    if (m_Level == 1) {
        genericInfantry.spriteRootPath = "../PTSD/assets/sprites/images/BarracksTower/Soldier/";
        genericInfantry.walkStart = 1;  genericInfantry.walkEnd = 6;
        genericInfantry.atkStart = 7;   genericInfantry.atkEnd = 17;
        genericInfantry.deadStart = 18; genericInfantry.deadEnd = 24;
    }
    else if (m_Level == 2) {
        genericInfantry.spriteRootPath = "../PTSD/assets/sprites/images/BarracksTower/TowerLevel2/Soldier/";
        // 對接二級小兵的 15 張圖配置
        genericInfantry.walkStart = 1;  genericInfantry.walkEnd = 6;   // 1 ~ 6 走路
        genericInfantry.atkStart = 7;   genericInfantry.atkEnd = 12;   // 7 ~ 12 攻擊
        genericInfantry.deadStart = 13; genericInfantry.deadEnd = 15;  // 13 ~ 15 死亡
    }
    else { // 等級三
        genericInfantry.spriteRootPath = "../PTSD/assets/sprites/images/BarracksTower/TowerLevel3/Soldier/";
        genericInfantry.walkStart = 1;  genericInfantry.walkEnd = 6;
        genericInfantry.atkStart = 7;   genericInfantry.atkEnd = 12;
        genericInfantry.deadStart = 13; genericInfantry.deadEnd = 15;
    }

    genericInfantry.maxHP = m_SoldierHP;
    genericInfantry.minDamage = m_SoldierDamage - 1;
    genericInfantry.maxDamage = m_SoldierDamage + 1;
    genericInfantry.speed = 1.5f;
    genericInfantry.detectionRange = 80.0f;
    genericInfantry.meleeRange = 20.0f;
    genericInfantry.attackCooldown = 0.6f;

    LOG_INFO("Spawn Soldier [Slot {}] - HP: {}, Atk: {}~{}", slotIndex, m_SoldierHP, genericInfantry.minDamage, genericInfantry.maxDamage);

    // 2. 直接生成
    m_Slots[slotIndex].soldier = std::make_shared<Soldier>(
        spawnPos,
        baseRallyPoint + formationOffset,
        genericInfantry
    );
}

void Barracks::Upgrade() {
    if (m_Level < m_MaxLevel) {
        m_Level++;

        // 套用下一級數據 (金額、範圍、冷卻)
        ApplyBaseStats(m_BarracksStats[m_Level - 1]);

        // 【核心修正】修正結構欄位名稱，即時更新塔本體造型圖片
        std::string newTowerPath = m_BarracksStats[m_Level - 1].baseSpritePath;
        SetDrawable(std::make_shared<Util::Image>(newTowerPath));

        // 🟢 修改為以下這段：精確還原當前等級的白板基礎值，再重新乘上天賦
        float baseHP = 50.0f + static_cast<float>(m_Level - 1) * 20.0f; // Lv1=50, Lv2=70, Lv3=90
        float baseDamage = m_BarracksStats[m_Level - 1].damage;

        // 升級時立即重生成活的小兵
        for (int i = 0; i < m_MaxSoldiers; ++i) {
            if (m_Slots[i].soldier) {
                m_Slots[i].soldier->ReleaseEnemy();
            }
            SpawnSoldier(i);
            m_Slots[i].isWaitingForRespawn = false;
        }

        LOG_INFO("Barracks Upgraded to Level {}!", m_Level);
    }
}

void Barracks::Draw() {
    // 1. 畫父類別的 UI (範圍圈、圓環、數字、升級/出售圖示)
    Tower::Draw();

    // 2. 畫塔建築物本體
    if (m_Visible && m_Drawable) {
        // 備份原始 transform，防止 m_Transform 被永久修改
        Util::Transform originalTransform = m_Transform;
        glm::vec2 size = m_Drawable->GetSize();

        // 調整臨時的變數值，而不改動真實的 m_Transform
        if (static_cast<int>(size.x) % 2 != 0) {
            m_Transform.translation.x = std::round(m_Transform.translation.x) + 0.5f;
        } else {
            m_Transform.translation.x = std::round(m_Transform.translation.x);
        }

        if (static_cast<int>(size.y) % 2 != 0) {
            m_Transform.translation.y = std::round(m_Transform.translation.y) + 0.5f;
        } else {
            m_Transform.translation.y = std::round(m_Transform.translation.y);
        }

        // 使用調整後的 transform 生成 buffer
        auto data = Util::ConvertToUniformBufferData(m_Transform, size, 15.0f);
        m_Drawable->Draw(data);

        // 畫完後立刻還原，這保證了 Barracks 邏輯層的座標永遠是正確的
        m_Transform = originalTransform;
    }

    // 3. 畫小兵
    for (int i = 0; i < m_MaxSoldiers; ++i) {
        if (m_Slots[i].soldier) {
            m_Slots[i].soldier->Draw();
        }
    }
}

void Barracks::UpdateAnimation() { }

void Barracks::Attack(std::shared_ptr<Enemy>, std::vector<std::shared_ptr<Enemy>>&, std::vector<std::shared_ptr<Projectile>>&) { }