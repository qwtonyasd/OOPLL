#ifndef APP_HPP
#define APP_HPP

#include <memory>
#include <vector>
#include "pch.hpp"
#include "TowerSlot.hpp"
#include "MapManager.hpp"
#include "Enemy.hpp"
#include "WorldMap.hpp"
#include "TowerManager.hpp"
#include "BuildMenu.hpp"

class App {
public:
    enum class State { START, UPDATE, END };
    State GetCurrentState() const { return m_CurrentState; }

    void Start();
    void Update(); // 確保宣告此函式以解決連結錯誤
    void End();    // 確保宣告此函式以解決連結錯誤

private:
    void ChangeLevel(int levelId);
    void HandleSelectLevel();
    void HandleGamePlay();

    // 遊戲物件管理
    std::unique_ptr<MapManager> m_MapManager;
    std::unique_ptr<WorldMap> m_WorldMap;
    std::unique_ptr<TowerManager> m_TowerManager;
    std::unique_ptr<BuildMenu> m_BuildMenu;

    // 塔位與敵人容器
    std::vector<std::shared_ptr<TowerSlot>> m_TowerSlots;
    std::vector<std::shared_ptr<Enemy>> m_Enemies;

    // 當前操作中的塔位
    std::shared_ptr<TowerSlot> m_ActiveSlot = nullptr;

    // 遊戲狀態變數
    State m_CurrentState = State::START;
    bool m_IsInGame = false;
    int m_MaxUnlockedLevel = 1;
    int m_CurrentLevelId = 1;
};

#endif