#ifndef MAP_MANAGER_HPP
#define MAP_MANAGER_HPP

#include <map>
#include <memory>
#include "Map.hpp"

class MapManager {
public:
    MapManager() = default;

    void AddLevel(int levelId, const std::shared_ptr<Map>& map) {
        m_Levels[levelId] = map;
        if (m_CurrentLevelId == -1) m_CurrentLevelId = levelId;
    }

    void SwitchLevel(int levelId) {
        if (m_Levels.count(levelId)) {
            m_CurrentLevelId = levelId;
        }
    }

    const std::vector<glm::vec2>& GetCurrentWaypoints() const {
        return m_Levels.at(m_CurrentLevelId)->GetWaypoints();
    }

    void Draw() {
        if (m_Levels.count(m_CurrentLevelId)) {
            // 直接呼叫 GameObject 的 Draw
            m_Levels[m_CurrentLevelId]->Draw();
        }
    }

private:
    std::map<int, std::shared_ptr<Map>> m_Levels;
    int m_CurrentLevelId = -1;
};

#endif