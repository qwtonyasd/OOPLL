#ifndef MAP_HPP
#define MAP_HPP

#include <vector>
#include <string>
#include <memory>
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

// 封裝每一關的靜態資料
struct LevelConfig {
    int levelId;
    std::string imagePath;
    std::vector<std::vector<glm::vec2>> routes; // 支援多條路徑
    std::vector<glm::vec2> towerSlotPositions;  // 該地圖的塔位座標
};

class Map : public Util::GameObject {
public:
    explicit Map(const LevelConfig& config) : m_Config(config) {
        m_Drawable = std::make_shared<Util::Image>(config.imagePath);
        m_ZIndex = -10.0f;
    }

    // OOP: 將「選路徑」的行為封裝在 Map 物件內部
    const std::vector<glm::vec2>& GetRandomRoute() const {
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