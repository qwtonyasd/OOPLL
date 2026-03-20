#ifndef CONTROL_STAGE_HPP
#define CONTROL_STAGE_HPP

#include <memory>
#include <vector>
#include "MapManager.hpp"
#include "EnemyFactory.hpp"
#include "MapFactory.hpp"
#include "TowerManager.hpp" // 必須引入

class ControlStage {
public:
    ControlStage() {
        m_MapManager = std::make_unique<MapManager>();
        m_TowerManager = std::make_unique<TowerManager>(); // 初始化塔管理員
    }

    void InitLevel(int levelId) {
        m_CurrentLevelId = levelId;
        auto newMap = MapFactory::CreateLevel(levelId);
        m_MapManager->AddLevel(levelId, newMap);
        m_MapManager->SwitchLevel(levelId);

        // 切換關卡時，清空場上的塔
        m_TowerManager->Clear();
    }

    // --- 新增：App.cpp 缺失的成員函數 ---
    void BuildTower(Tower::Type type, const glm::vec2& pos) {
        m_TowerManager->AddTower(type, pos);
    }

    void UpdateTowers(std::vector<std::shared_ptr<Enemy>>& enemies) {
        m_TowerManager->UpdateAll(enemies);
    }

    void DrawTowers() {
        m_TowerManager->DrawAll();
    }
    // ----------------------------------

    void SpawnMonster(EnemyFactory::Type type, std::vector<std::shared_ptr<Enemy>>& container) {
        auto currentMap = m_MapManager->GetCurrentMap();
        if (currentMap) {
            const auto& route = currentMap->GetRandomRoute();
            auto newEnemy = EnemyFactory::Create(type, route);
            container.push_back(newEnemy);
        }
    }

    std::shared_ptr<Map> GetCurrentMap() { return m_MapManager->GetCurrentMap(); }
    void DrawMap() { m_MapManager->Draw(); }

private:
    std::unique_ptr<MapManager> m_MapManager;
    std::unique_ptr<TowerManager> m_TowerManager; // 持有 TowerManager
    int m_CurrentLevelId = -1;
};

#endif