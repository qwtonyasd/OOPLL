#ifndef BARRACKS_H
#define BARRACKS_H

#include "Tower.hpp"
#include "Soldier.hpp"
#include <vector>
#include <memory>
#include <string>
#include "GameData.hpp"
class Barracks : public Tower {
public:
    int GetTotalCost() const override {
        // 根據當前等級（m_Level），精確回傳該階段累積的總花費
        switch (m_Level) {
            case 1: return 70;                  // 1等造價
            case 2: return 70 + 110;            // 1等 + 2等升級費 (180)
            case 3: return 70 + 110 + 160;      // 1等 + 2等 + 3等升級費 (340)
            default: return 70;
        }
    }

    Barracks(glm::vec2 pos, const std::vector<glm::vec2>& route);

    void Update(std::vector<std::shared_ptr<Enemy>>& enemies,
                std::vector<std::shared_ptr<Projectile>>& projectiles) override;
    void Upgrade() override;
    void Attack(std::shared_ptr<Enemy> target,
                std::vector<std::shared_ptr<Enemy>>& allEnemies,
                std::vector<std::shared_ptr<Projectile>>& projectiles) override;

    void UpdateAnimation() override;
    void Draw() override;
    // 在 BombTower.h 或是對應的標頭檔
    struct BombStats : public TowerStats {
        std::string towerAnimFolder; // 塔動畫資料夾路徑
        std::string bulletSprite;    // 子彈圖片路徑
        std::string explosionFolder; // 爆炸動畫資料夾路徑
    };
    void SpawnSoldier(int slotIndex);

private:
    struct SoldierSlot {
        std::shared_ptr<Soldier> soldier = nullptr;
        float respawnTimer = 0.0f;
        bool isWaitingForRespawn = false;
    };

    // --- 數據管理 ---
    std::vector<TowerStats> m_BarracksStats;
    void LoadLevelAssets(); // 用於切換塔的外觀

    SoldierSlot m_Slots[3];
    std::vector<glm::vec2> m_Route;
    int m_MaxSoldiers = 3;

    // 小兵屬性隨等級提升
    float m_SoldierHP = 50.0f;
    float m_SoldierDamage = 3.0f;
};
#endif