#ifndef APP_HPP
#define APP_HPP

#include <memory>
#include <vector>
#include "pch.hpp"
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
    void ValidTask();

private:
    std::unique_ptr<MapManager> m_MapManager;
    State m_CurrentState = State::START;
};

#endif