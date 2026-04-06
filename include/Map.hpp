#ifndef MAP_HPP
#define MAP_HPP

#include <vector>
#include <string>
#include <memory>
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

struct LevelConfig {
    int levelId;
    std::string imagePath;
    std::vector<std::vector<glm::vec2>> routes;
    std::vector<glm::vec2> towerSlotPositions;
};

class Map : public Util::GameObject {
public:
    explicit Map(const LevelConfig& config) : m_Config(config) {
        m_Drawable = std::make_shared<Util::Image>(config.imagePath);
        m_ZIndex = -10.0f;
        m_Transform.translation = {0, 0};
        m_Transform.scale = {1.0f, 1.0f}; // 保持 1.0，不強行拉伸以免座標偏移
    }

    const std::vector<glm::vec2>& GetRandomRoute() const {
        int index = rand() % m_Config.routes.size();
        return m_Config.routes[index];
    }

    // 直接回傳原始座標清單
    const std::vector<glm::vec2>& GetTowerSlots() const {
        return m_Config.towerSlotPositions;
    }

private:
    LevelConfig m_Config;
};

#endif