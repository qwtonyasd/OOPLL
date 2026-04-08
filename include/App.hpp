#ifndef APP_HPP
#define APP_HPP

#include <memory>
#include <vector>
#include "Util/GameObject.hpp"
#include "MapManager.hpp"
#include "TowerManager.hpp"
#include "BuildMenu.hpp"
#include "Hud.hpp"
#include "WorldMap.hpp"
#include "TowerSlot.hpp"
#include "Enemy.hpp"

class App {
public:
    enum class State { START, UPDATE, END };

    App() = default;
    void Start();
    void Update();
    void End();
    State GetState() const { return m_CurrentState; }

private:
    void HandleSelectLevel();
    void HandleGamePlay();
    void ChangeLevel(int levelId);

    // 渲染與邏輯組件
    Util::GameObject m_Root;
    std::unique_ptr<MapManager> m_MapManager;
    std::unique_ptr<TowerManager> m_TowerManager;
    std::unique_ptr<BuildMenu> m_BuildMenu;
    std::unique_ptr<Hud> m_Hud;
    std::unique_ptr<WorldMap> m_WorldMap;

    // 遊戲物件容器
    std::vector<std::shared_ptr<TowerSlot>> m_TowerSlots;
    std::vector<std::shared_ptr<Enemy>> m_Enemies;
    std::shared_ptr<TowerSlot> m_SelectedSlot = nullptr;

    State m_CurrentState = State::START;
    bool m_IsInGame = false;
};

#endif