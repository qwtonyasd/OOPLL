#ifndef TOWER_MANAGER_HPP
#define TOWER_MANAGER_HPP

#include "Util/Logger.hpp"  // 必須放在最前面
#include "Tower.hpp"
#include "tower/ArcherTower.h"
#include "tower/MageTower.h"
#include "tower/BombTower.h"
#include "tower/Barracks.h"

class TowerManager {
public:
    void AddTower(Tower::Type type, glm::vec2 pos) {
        if (type == Tower::Type::NONE) return;

        std::shared_ptr<Tower> newTower = nullptr;
        if (type == Tower::Type::ARCHER) newTower = std::make_shared<ArcherTower>(pos);
        else if (type == Tower::Type::MAGE) newTower = std::make_shared<MageTower>(pos);
        else if (type == Tower::Type::BOMB) newTower = std::make_shared<BombTower>(pos);
        else if (type == Tower::Type::BARRACKS) newTower = std::make_shared<Barracks>(pos);

        if (newTower) {
            m_Towers.push_back(newTower);
            LOG_INFO("Tower added! Total towers: {}", m_Towers.size());
        }
    }

    void UpdateAll(std::vector<std::shared_ptr<Enemy>>& enemies) {
        for (auto& tower : m_Towers) tower->Update(enemies);
    }

    void DrawAll() {
        for (auto& tower : m_Towers) tower->Draw();
    }

    void Clear() { m_Towers.clear(); }

private:
    std::vector<std::shared_ptr<Tower>> m_Towers;
};
#endif