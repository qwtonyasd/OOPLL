#ifndef BARRACKS_HPP
#define BARRACKS_HPP

#include "Tower.hpp"

class Barracks : public Tower {
public:
    Barracks(glm::vec2 pos)
        : Tower(pos, "../PTSD/assets/sprites/images/BarracksTower/1.png", 150.0f, 4.0f, 5.0f) {}

    void Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) override {
        (void)allEnemies; (void)target;
        LOG_INFO("Barracks spawning/rallying soldiers!");
        // 兵營特殊邏輯：召喚 3 個 Soldier 物件到指定路徑點
    }
};

#endif