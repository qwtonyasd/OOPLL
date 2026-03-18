#ifndef CONTROL_STAGE_HPP
#define CONTROL_STAGE_HPP

#include <memory>
#include <vector>
#include "MapManager.hpp"
#include "EnemyFactory.hpp"
#include "MapFactory.hpp"

class ControlStage {
public:
    ControlStage() {
        m_MapManager = std::make_unique<MapManager>();
    }

    // 當玩家在主畫面選關時呼叫
    void InitLevel(int levelId) {
        m_CurrentLevelId = levelId;
        // 透過 MapFactory 生成關卡物件並交給 Manager
        auto newMap = MapFactory::CreateLevel(levelId);
        m_MapManager->AddLevel(levelId, newMap);
        m_MapManager->SwitchLevel(levelId);
    }

    // 控制 EnemyFactory 生成怪物，並從 MapManager 拿取隨機路徑
    void SpawnMonster(EnemyFactory::Type type, std::vector<std::shared_ptr<Enemy>>& container) {
        auto currentMap = m_MapManager->GetCurrentMap();
        if (currentMap) {
            // 從地圖物件中取得隨機一條路徑
            const auto& route = currentMap->GetRandomRoute();
            
            // 叫工廠做出怪物實體
            auto newEnemy = EnemyFactory::Create(type, route);
            
            // 丟回給 App 維護的容器
            container.push_back(newEnemy);
        }
    }

    // 輔助函式：讓 App 能拿到地圖資訊來畫塔位
    std::shared_ptr<Map> GetCurrentMap() { return m_MapManager->GetCurrentMap(); }

    void DrawMap() { m_MapManager->Draw(); }

private:
    std::unique_ptr<MapManager> m_MapManager;
    int m_CurrentLevelId = -1;
};

#endif