#ifndef BARRACKS_H
#define BARRACKS_H

#include "Tower.hpp"
#include "Soldier.hpp"
#include <vector>

class Barracks : public Tower {
public:
    Barracks(glm::vec2 pos, const std::vector<glm::vec2>& route);

    void Update(std::vector<std::shared_ptr<Enemy>>& enemies,
                std::vector<std::shared_ptr<Projectile>>& projectiles) override;

    // 參數必須完全對齊
    void Attack(std::shared_ptr<Enemy> target,
                std::vector<std::shared_ptr<Enemy>>& allEnemies,
                std::vector<std::shared_ptr<Projectile>>& projectiles) override;

    void Draw() override;
    void SpawnSoldier();

private:
    std::vector<std::shared_ptr<Soldier>> m_Soldiers;
    std::vector<glm::vec2> m_Route;
    int m_MaxSoldiers = 3;
    float m_SpawnTimer = 0.0f;
};

#endif