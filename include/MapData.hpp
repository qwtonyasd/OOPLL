#ifndef MAP_DATA_HPP
#define MAP_DATA_HPP

#include <glm/vec2.hpp>
#include <map>
#include <vector>
#include <cmath>

struct Vec2Comparator {
    bool operator()(const glm::vec2& a, const glm::vec2& b) const {
        if (std::abs(a.x - b.x) > 1.0f) return a.x < b.x;
        if (std::abs(a.y - b.y) > 1.0f) return a.y < b.y;
        return false;
    }
};

class MapData {
public:
    static inline std::map<glm::vec2, glm::vec2, Vec2Comparator> TowerRallyMap = {
        // 請在此填入您的量測座標
        {{-156.0f, 102.0f}, {-130.0f, 20.0f}},
        {{50.0f, 150.0f},   {80.0f, 80.0f}}
    };

    // 關鍵修正：確保名稱與 Barracks.cpp 呼叫的一致，且必須是 static
    static glm::vec2 GetBaseRallyPoint(glm::vec2 towerPos) {
        for (auto const& [pos, rally] : TowerRallyMap) {
            if (glm::distance(towerPos, pos) < 10.0f) {
                return rally;
            }
        }
        return towerPos + glm::vec2(0, -60); // 預設回退方案
    }
};

#endif