// TowerManager.hpp
#ifndef TOWER_MANAGER_HPP
#define TOWER_MANAGER_HPP

#include "Tower.hpp"

class TowerManager {
public:
    void AddTower(Tower::Type type, glm::vec2 pos) {
        if (type == Tower::Type::ARCHER)
            m_Towers.push_back(std::make_shared<ArcherTower>(pos));
        // ... 其他塔類型的 switch case
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