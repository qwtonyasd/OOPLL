#ifndef APP_HPP
#define APP_HPP

#include <memory>
#include <vector>
#include "pch.hpp"
#include "TowerSlot.hpp"
#include "ControlStage.hpp" // 引入新的大腦
#include "Enemy.hpp"        // 引入怪物基類

class App {
public:
    enum class State { START, UPDATE, END };
    State GetCurrentState() const { return m_CurrentState; }

    void Start();
    void Update();
    void End();

private:
    void ChangeLevel(int levelId);

    std::unique_ptr<ControlStage> m_ControlStage;      // 關卡大腦
    std::vector<std::shared_ptr<TowerSlot>> m_TowerSlots;
    std::vector<std::shared_ptr<Enemy>> m_Enemies;     // 怪物容器 (修復紅字)

    State m_CurrentState = State::START;
};

#endif
