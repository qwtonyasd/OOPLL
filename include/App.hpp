#ifndef APP_HPP
#define APP_HPP

#include <memory>
#include <vector>
#include "pch.hpp"
#include "TowerSlot.hpp"
#include "ControlStage.hpp"
#include "Enemy.hpp"
#include "Tower.hpp"        // 引入 Tower 以便辨識類型

class App {
public:
    enum class State { START, UPDATE, END };
    State GetCurrentState() const { return m_CurrentState; }

    void Start();
    void Update();
    void End();

private:
    void ChangeLevel(int levelId);

    std::unique_ptr<ControlStage> m_ControlStage;
    std::vector<std::shared_ptr<TowerSlot>> m_TowerSlots;
    std::vector<std::shared_ptr<Enemy>> m_Enemies;

    // --- 新增：追蹤目前滑鼠懸停的塔位 ---
    std::shared_ptr<TowerSlot> m_HoveredSlot = nullptr;

    State m_CurrentState = State::START;
};

#endif