#ifndef MAP_MANAGER_HPP
#define MAP_MANAGER_HPP

#include <map>
#include <memory>
#include "Map.hpp"

class MapManager {
public:
    MapManager() = default;

    // 現在直接傳入 Map 物件
    void AddLevel(int levelId, const std::shared_ptr<Map>& map) {
        m_Levels[levelId] = map;
        if (m_CurrentLevelId == -1) m_CurrentLevelId = levelId;
    }

    void SwitchLevel(int levelId) {
        if (m_Levels.count(levelId)) {
            m_CurrentLevelId = levelId;
        }
    }

    // 取得當前關卡的隨機路線（給哥布林用）
    const std::vector<glm::vec2>& GetRandomRoute() const {
        return m_Levels.at(m_CurrentLevelId)->GetRandomRoute();
    }

    // 取得當前關卡的塔位座標
    const std::vector<glm::vec2>& GetTowerSlots() const {
        return m_Levels.at(m_CurrentLevelId)->GetTowerSlots();
    }

    void Draw() {
        if (m_Levels.count(m_CurrentLevelId)) {
            m_Levels[m_CurrentLevelId]->Draw();
        }
    }

    std::shared_ptr<Map> GetCurrentMap() { return m_Levels[m_CurrentLevelId]; }

private:
    std::map<int, std::shared_ptr<Map>> m_Levels;
    int m_CurrentLevelId = -1;
};

#endif
