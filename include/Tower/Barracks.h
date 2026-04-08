#ifndef BARRACKS_H
#define BARRACKS_H

#include "Tower.hpp"
#include "Soldier.hpp"
#include <vector>
#include <memory>

class Barracks : public Tower {
public:
    Barracks(glm::vec2 pos, const std::vector<glm::vec2>& route);

    // 必須實作 Attack，否則無法編譯
    void Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) override;

    void Update(std::vector<std::shared_ptr<Enemy>>& enemies) override;
    void Draw() override;

private:
    void SpawnSoldier();
    std::vector<std::shared_ptr<Soldier>> m_Soldiers;
    std::vector<glm::vec2> m_Route;
    float m_SpawnTimer = 0.0f;
};

#endif