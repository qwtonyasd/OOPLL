#ifndef BARRACKS_H
#define BARRACKS_H

#include "Tower.hpp"
#include "Soldier.hpp"
#include <vector>
#include <memory>

class Barracks : public Tower {
public:
    Barracks(glm::vec2 pos, const std::vector<glm::vec2>& route);

    void Update(std::vector<std::shared_ptr<Enemy>>& enemies,
                std::vector<std::shared_ptr<Projectile>>& projectiles) override;

    void Attack(std::shared_ptr<Enemy> target,
                std::vector<std::shared_ptr<Enemy>>& allEnemies,
                std::vector<std::shared_ptr<Projectile>>& projectiles) override;

    void Draw() override;

    // 修改：SpawnSoldier 現在需要知道是補哪一個位置
    void SpawnSoldier(int slotIndex);

private:
    // 新增：席位結構，獨立管理每個小兵的生存狀態與計時
    struct SoldierSlot {
        std::shared_ptr<Soldier> soldier = nullptr;
        float respawnTimer = 0.0f;
        bool isWaitingForRespawn = false;
    };

    SoldierSlot m_Slots[3]; // 固定 3 個席位
    std::vector<glm::vec2> m_Route;
    int m_MaxSoldiers = 3;
    // 原本全域的 m_SpawnTimer 已被移除
};
#endif