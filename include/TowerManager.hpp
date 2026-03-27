// TowerManager.hpp 修正版
#ifndef TOWER_MANAGER_HPP
#define TOWER_MANAGER_HPP

#include "Tower.hpp"               // 基類
#include "tower/ArcherTower.h"    // 新路徑
#include "tower/MageTower.h"      // 新路徑
#include "tower/BombTower.h"      // 新路徑
#include "tower/Barracks.h"       // 新路徑

class TowerManager {
public:
    void AddTower(Tower::Type type, glm::vec2 pos) {
        if (type == Tower::Type::NONE) return;

        if (type == Tower::Type::ARCHER) {
            m_Towers.push_back(std::make_shared<ArcherTower>(pos));
        }
        else if (type == Tower::Type::MAGE) {
            m_Towers.push_back(std::make_shared<MageTower>(pos));
        }
        else if (type == Tower::Type::BOMB) {
            m_Towers.push_back(std::make_shared<BombTower>(pos));
        }
        else if (type == Tower::Type::BARRACKS) {
            m_Towers.push_back(std::make_shared<Barracks>(pos));
        }

        LOG_INFO("Tower added! Total towers: {}", m_Towers.size());
    }

    void UpdateAll(std::vector<std::shared_ptr<Enemy>>& enemies) {
        for (auto& tower : m_Towers) {
            tower->Update(enemies);
        }
    }

    void DrawAll() {
        for (auto& tower : m_Towers) {
            tower->Draw();
        }
    }

    void Clear() { m_Towers.clear(); }

private:
    std::vector<std::shared_ptr<Tower>> m_Towers;
};
#endif