#ifndef MAP_HPP
#define MAP_HPP

#include <vector>
#include <memory>
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

class Map : public Util::GameObject {
public:
    // 使用 glm::vec2 作為路徑點類型
    Map(const std::string& imagePath, const std::vector<glm::vec2>& waypoints)
        : m_Waypoints(waypoints) {

        // 1. 設定圖片 (m_Drawable 是繼承自 GameObject 的 protected 成員)
        m_Drawable = std::make_shared<Util::Image>(imagePath);

        // 2. 設定 Z 軸 (地圖通常在最後面，設一個較小的負數或 0)
        m_ZIndex = -10.0f;

        // 3. 確保位置在中心
        m_Transform.translation = {0, 0};
    }

    // 取得該地圖的路徑點
    const std::vector<glm::vec2>& GetWaypoints() const { return m_Waypoints; }

private:
    std::vector<glm::vec2> m_Waypoints;
};

#endif