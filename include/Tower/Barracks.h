#ifndef BARRACKS_HPP
#define BARRACKS_HPP

#include "Tower.hpp"
#include "Soldier.hpp"
#include <vector>
#include <memory>

class Barracks : public Tower {
public:
    Barracks(glm::vec2 pos);

    // 實作父類的虛擬函式
    void Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) override;
    void Update(std::vector<std::shared_ptr<Enemy>>& enemies) override;
    void Draw() override;

private:
    std::vector<std::shared_ptr<Soldier>> m_Soldiers;
    float m_RespawnTimer;
    const float RESPAWN_COOLDOWN = 10.0f; // 重生冷卻時間
};

#endif