#ifndef MAP_HPP
#define MAP_HPP

#include <vector>
#include <string>
#include <memory>
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

// 封裝每一關的靜態資料，包含 4 條路徑與塔位
struct LevelConfig {
    int levelId;
    std::string imagePath;
    std::vector<std::vector<glm::vec2>> routes; // 這裡改為二維，支援隨機 4 條路徑
    std::vector<glm::vec2> towerSlotPositions;  // 該地圖的塔位座標
};

class Map : public Util::GameObject {
public:
    explicit Map(const LevelConfig& config) : m_Config(config) {
        m_Drawable = std::make_shared<Util::Image>(config.imagePath);
        m_ZIndex = -10.0f; // 地圖設在最底層
        m_Transform.translation = {0, 0}; // 確保地圖置中
    }

    // 從 4 條預設路徑中隨機挑選一條給哥布林使用
    const std::vector<glm::vec2>& GetRandomRoute() const {
        if (m_Config.routes.empty()) {
            static std::vector<glm::vec2> empty;
            return empty;
        }
        int index = rand() % m_Config.routes.size();
        return m_Config.routes[index];
    }

    const std::vector<glm::vec2>& GetTowerSlots() const {
        return m_Config.towerSlotPositions;
    }

private:
    LevelConfig m_Config;
};

#endif