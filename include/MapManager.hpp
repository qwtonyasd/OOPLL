#ifndef MAP_MANAGER_HPP
#define MAP_MANAGER_HPP

#include "Map.hpp"
#include "MapFactory.hpp"

class MapManager {
public:
    MapManager() = default;

    void LoadLevel(int levelId) {
        // 直接透過工廠取得物件
        m_CurrentMap = MapFactory::CreateLevel(levelId);
    }

    void Draw() {
        if (m_CurrentMap) {
            m_CurrentMap->Draw();
        }
    }

    std::shared_ptr<Map> GetCurrentMap() const { return m_CurrentMap; }

private:
    std::shared_ptr<Map> m_CurrentMap;
};

#endif