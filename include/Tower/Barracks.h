#ifndef BARRACKS_H
#define BARRACKS_H

#include "Tower.hpp"
#include "Soldier.hpp"
#include <vector>
#include <memory>

class Barracks : public Tower {
public:
    Barracks(glm::vec2 pos, const std::vector<glm::vec2>& route);

    // 確保這裡的參數數量與 Tower.hpp 的虛擬函式完全一致
    void Update(std::vector<std::shared_ptr<Enemy>>& enemies,
                std::vector<std::shared_ptr<Projectile>>& projectiles) override;
    void Upgrade() override;
    void Attack(std::shared_ptr<Enemy> target,
                std::vector<std::shared_ptr<Enemy>>& allEnemies,
                std::vector<std::shared_ptr<Projectile>>& projectiles) override;

    void UpdateAnimation() override; // 即使沒動畫也要 override 避免變抽象類
    void Draw() override;

    void SpawnSoldier(int slotIndex);

private:
    struct SoldierSlot {
        std::shared_ptr<Soldier> soldier = nullptr;
        float respawnTimer = 0.0f;
        bool isWaitingForRespawn = false;
    };

    SoldierSlot m_Slots[3];
    std::vector<glm::vec2> m_Route;
    int m_MaxSoldiers = 3;
};
#endif