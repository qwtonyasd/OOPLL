#ifndef CONTROL_STAGE_HPP
#define CONTROL_STAGE_HPP

#include <memory>
#include <vector>
#include "MapManager.hpp"
#include "EnemyFactory.hpp"
#include "MapFactory.hpp"
#include "TowerManager.hpp"
#include "tower/Soldier.hpp"

class ControlStage {
public:
    ControlStage() {
        m_MapManager = std::make_unique<MapManager>();
        m_TowerManager = std::make_unique<TowerManager>();
    }

    void InitLevel(int levelId) {
        m_CurrentLevelId = levelId;
        auto newMap = MapFactory::CreateLevel(levelId);
        m_MapManager->AddLevel(levelId, newMap);
        m_MapManager->SwitchLevel(levelId);

        m_TowerManager->Clear();
        m_ActiveReinforcements.clear(); // 切關卡時清空援軍
    }

    void BuildTower(Tower::Type type, const glm::vec2& pos) {
        m_TowerManager->AddTower(type, pos);
    }

    // 供 SpellManager 取得法術小兵陣列的引用
    std::vector<std::shared_ptr<Soldier>>& GetActiveReinforcements() {
        return m_ActiveReinforcements;
    }

    // 每影格核心邏輯更新
    void Update(std::vector<std::shared_ptr<Enemy>>& enemies, float dt) {
        // 1. 更新所有塔
        m_TowerManager->UpdateAll(enemies);

        // 2. 更新所有法術召喚出來的民兵
        for (auto it = m_ActiveReinforcements.begin(); it != m_ActiveReinforcements.end(); ) {
            auto& soldier = *it;
            if (soldier) {
                soldier->Update(enemies, dt);

                if (soldier->GetHP() <= 0 && soldier->IsDeadAnimationFinished()) {
                    soldier->ReleaseEnemy();
                    it = m_ActiveReinforcements.erase(it);
                    continue;
                }
            }
            ++it;
        }
    }

    // 核心繪製邏輯
    void Draw() {
        m_MapManager->Draw();       // 1. 地圖
        m_TowerManager->DrawAll();  // 2. 塔建築

        for (auto& soldier : m_ActiveReinforcements) { // 3. 援軍
            if (soldier) soldier->Draw();
        }
    }

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
    void DrawTowers() { m_TowerManager->DrawAll(); }
    void UpdateTowers(std::vector<std::shared_ptr<Enemy>>& enemies) { m_TowerManager->UpdateAll(enemies); }

private:
    std::unique_ptr<MapManager> m_MapManager;
    std::unique_ptr<TowerManager> m_TowerManager;
    int m_CurrentLevelId = -1;

    // 僅保留法術小兵的生命容器
    std::vector<std::shared_ptr<Soldier>> m_ActiveReinforcements;
};

#endif