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
#include "Projectile.hpp"
#include "VictoryMenu.hpp" // 1. 補上勝利選單標頭檔

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

    // 波次資料結構
    struct WaveConfig {
        std::vector<Enemy::Type> enemyList;
    };

    void InitWaveData(); // 初始化 7 個波次

    // 核心組件
    Util::GameObject m_Root;
    std::unique_ptr<MapManager> m_MapManager;
    std::unique_ptr<TowerManager> m_TowerManager;
    std::unique_ptr<BuildMenu> m_BuildMenu;
    std::unique_ptr<Hud> m_Hud;
    std::unique_ptr<WorldMap> m_WorldMap;

    // 2. 補上勝利選單組件
    std::unique_ptr<VictoryMenu> m_VictoryMenu;

    // 物件容器
    std::vector<std::shared_ptr<TowerSlot>> m_TowerSlots;
    std::vector<std::shared_ptr<Enemy>> m_Enemies;
    std::vector<std::shared_ptr<Projectile>> m_Projectiles;
    std::shared_ptr<TowerSlot> m_SelectedSlot = nullptr;

    // 波次控制變數
    std::vector<WaveConfig> m_Waves;
    int m_SpawnIndex = 0;          // 當前波次已生成的怪編號
    float m_SpawnTimer = 0.0f;     // 兩隻怪之間的間隔
    float m_WaveBreakTimer = 0.0f; // 波次與波次之間的休息時間
    bool m_IsWaveActive = false;   // 正在出兵中

    // 3. 補上當前關卡 ID 紀錄
    int m_CurrentLevelID = 1;

    State m_CurrentState = State::START;
    bool m_IsInGame = false;
};

#endif