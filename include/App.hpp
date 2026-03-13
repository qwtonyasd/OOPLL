#ifndef APP_HPP
#define APP_HPP

#include <memory>
#include <vector>
#include "pch.hpp"
#include "TowerSlot.hpp"
#include "MapManager.hpp"

class App {
public:
    enum class State {
        START,
        UPDATE,
        END,
    };

    State GetCurrentState() const { return m_CurrentState; }

    void Start();
    void Update();
    void End();

private:
    /**
     * @brief 核心 OOP 方法：統一處理關卡切換
     * 負責叫 MapManager 換地圖，並重新同步生成 m_TowerSlots
     */
    void ChangeLevel(int levelId);

    std::vector<std::shared_ptr<TowerSlot>> m_TowerSlots;
    std::unique_ptr<MapManager> m_MapManager;
    State m_CurrentState = State::START;
};

#endif