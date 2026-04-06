#ifndef APP_HPP
#define APP_HPP

#include <memory>
#include <vector>
#include "MapManager.hpp"
#include "WorldMap.hpp"
#include "TowerManager.hpp"
#include "BuildMenu.hpp"
#include "TowerSlot.hpp"
#include "Enemy.hpp"
#include "Hud.hpp"

class App {
public:
    enum class State { START, UPDATE, END };
    void Start();
    void Update();
    void End();
    State GetState() const { return m_CurrentState; }

private:
    void HandleSelectLevel();
    void HandleGamePlay();
    void ChangeLevel(int levelId);

    State m_CurrentState = State::START;
    bool m_IsInGame = false;

    std::unique_ptr<MapManager> m_MapManager;
    std::unique_ptr<WorldMap> m_WorldMap;
    std::unique_ptr<TowerManager> m_TowerManager;
    std::unique_ptr<BuildMenu> m_BuildMenu;
    std::unique_ptr<Hud> m_Hud;

    std::vector<std::shared_ptr<TowerSlot>> m_TowerSlots;
    std::vector<std::shared_ptr<Enemy>> m_Enemies;

    // 儲存目前選中的塔位，用於蓋塔
    std::shared_ptr<TowerSlot> m_SelectedSlot;
};

#endif