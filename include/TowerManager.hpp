#ifndef TOWERMANAGER_HPP
#define TOWERMANAGER_HPP

#include <vector>
#include <memory>
#include "Tower.hpp"
#include "tower/Barracks.h"
#include "tower/ArcherTower.h"
#include "tower/MageTower.h"
#include "tower/BombTower.h"
#include "Util/GameObject.hpp"
#include "Util/Logger.hpp"

class TowerManager {
public:
    TowerManager(Util::GameObject& root) : m_Root(root) {}

    void AddTower(Tower::Type type, glm::vec2 pos, const std::vector<glm::vec2>& route) {
        std::shared_ptr<Tower> newTower = nullptr;

        // 嚴格對應：確保類別名稱與 Type 一致
        switch (type) {
            case Tower::Type::ARCHER:
                newTower = std::make_shared<ArcherTower>(pos);
                break;
            case Tower::Type::BARRACKS:
                newTower = std::make_shared<Barracks>(pos, route);
                break;
            case Tower::Type::MAGE:
                newTower = std::make_shared<MageTower>(pos);
                break;
            case Tower::Type::BOMB:
                newTower = std::make_shared<BombTower>(pos);
                break;
            default:
                LOG_ERROR("Unknown Tower Type!");
                return;
        }

        if (newTower) {
            m_Towers.push_back(newTower);
            m_Root.AddChild(newTower); // 加入場景樹自動渲染
            LOG_INFO("Tower Created: Type {}", static_cast<int>(type));
        }
    }

    void UpdateAll(std::vector<std::shared_ptr<Enemy>>& enemies) {
        for (auto& tower : m_Towers) tower->Update(enemies);
    }

    void DrawAll() {
        for (auto& tower : m_Towers) tower->Draw();
    }

    void Clear() {
        for (auto& tower : m_Towers) m_Root.RemoveChild(tower);
        m_Towers.clear();
    }

private:
    std::vector<std::shared_ptr<Tower>> m_Towers;
    Util::GameObject& m_Root;
};

#endif