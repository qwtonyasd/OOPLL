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
#include "Tower/Projectile/Projectile.hpp"
#include "VictoryMenu.hpp"
#include "Map.hpp"

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

    // 核心組件
    Util::GameObject m_Root;
    std::unique_ptr<MapManager> m_MapManager;
    std::unique_ptr<TowerManager> m_TowerManager;
    std::unique_ptr<BuildMenu> m_BuildMenu;
    std::unique_ptr<Hud> m_Hud;
    std::unique_ptr<WorldMap> m_WorldMap;
    std::unique_ptr<VictoryMenu> m_VictoryMenu;

    // 物件容器
    std::vector<std::shared_ptr<TowerSlot>> m_TowerSlots;
    std::vector<std::shared_ptr<Enemy>> m_Enemies;
    std::vector<std::shared_ptr<Projectile>> m_Projectiles;
    std::shared_ptr<TowerSlot> m_SelectedSlot = nullptr;

    // 當前關卡的波次配置與暫存
    std::vector<WaveConfig> m_Waves;
    std::vector<SubWave> m_PendingSubWaves; // 尚未生成的怪物清單

    // 波次控制變數
    float m_WaveTimer = 0.0f;       // 波次進行時間
    float m_WaveBreakTimer = 0.0f;  // 波次間隔計時
    bool m_IsWaveActive = false;    // 波次是否進行中

    int m_CurrentLevelID = 1;
    State m_CurrentState = State::START;
    bool m_IsInGame = false;
};

#endif