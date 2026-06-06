#ifndef APP_HPP
#define APP_HPP

#include <memory>
#include <vector>
#include "Util/GameObject.hpp"
#include "Util/BGM.hpp" // 🎯 新增：引入 PTSD 的背景音樂類別
#include "MapManager.hpp"
#include "TowerManager.hpp"
#include "BuildMenu.hpp"
#include "Hud.hpp"
#include "WorldMap.hpp"
#include "TowerSlot.hpp"
#include "Enemy.hpp"
#include "Tower/Projectile/Projectile.hpp"
#include "VictoryMenu.hpp"
#include "UpgradeMenu.hpp"
#include "Map.hpp"

// 法術系統與小兵的標頭檔
#include "MageSkill/SpellManager.hpp"
#include "Soldier.hpp"

class App {
public:
    enum class State { START, UPDATE, END };

    // 🎯 新增：用於音樂控制的內部場景狀態
    enum class MusicState { MAIN_MENU, GAME_PLAY, END_GAME };

    App() = default;
    void Start();
    void Update();
    void End();
    State GetState() const { return m_CurrentState; }

private:
    void HandleSelectLevel();
    void HandleGamePlay();
    void ChangeLevel(int levelId);
    void ChangeMusic(MusicState targetMusic); // 🎯 新增：安全切換音樂的輔助函式

    // 核心組件
    Util::GameObject m_Root;
    std::unique_ptr<MapManager> m_MapManager;
    std::unique_ptr<TowerManager> m_TowerManager;
    std::unique_ptr<BuildMenu> m_BuildMenu;
    std::unique_ptr<Hud> m_Hud;
    std::unique_ptr<WorldMap> m_WorldMap;
    std::unique_ptr<VictoryMenu> m_VictoryMenu;
    std::unique_ptr<UpgradeMenu> m_UpgradeMenu;

    // 主動法術管理器
    std::unique_ptr<SpellManager> m_SpellManager;

    // 🎯 新增：三首不同情境的背景音樂管理物件
    std::shared_ptr<Util::BGM> m_BgmMainMenu;
    std::shared_ptr<Util::BGM> m_BgmUnderAttack;
    std::shared_ptr<Util::BGM> m_BgmEndGame;
    MusicState m_CurrentMusicState = MusicState::MAIN_MENU; // 記錄當前正在播放哪首音樂

    // 物件容器
    std::vector<std::shared_ptr<TowerSlot>> m_TowerSlots;
    std::vector<std::shared_ptr<Enemy>> m_Enemies;
    std::vector<std::shared_ptr<Projectile>> m_Projectiles;

    // 存放法術召喚出的民兵/援軍容器
    std::vector<std::shared_ptr<Soldier>> m_ActiveReinforcements;

    std::shared_ptr<TowerSlot> m_SelectedSlot = nullptr;

    // 當前關卡的波次配置與暫存
    std::vector<WaveConfig> m_Waves;
    std::vector<SubWave> m_PendingSubWaves;

    // 波次控制變數
    float m_WaveTimer = 0.0f;
    float m_WaveBreakTimer = 0.0f;
    bool m_IsWaveActive = false;

    int m_CurrentLevelID = 1;
    State m_CurrentState = State::START;
    bool m_IsInGame = false;
};

#endif // APP_HPP