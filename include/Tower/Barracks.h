#ifndef BARRACKS_HPP
#define BARRACKS_HPP

#include "Tower.hpp"
#include "Soldier.hpp"
#include <vector>
#include <memory>
#include <glm/glm.hpp>

class Barracks : public Tower {
public:
    // 修改建構子，增加路徑參數
    Barracks(glm::vec2 pos, const std::vector<glm::vec2>& route);

    void Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) override;
    void Update(std::vector<std::shared_ptr<Enemy>>& enemies) override;
    void Draw() override;

private:
    std::vector<std::shared_ptr<Soldier>> m_Soldiers;
    float m_RespawnTimer;
    const float RESPAWN_COOLDOWN = 10.0f;
};

#endif