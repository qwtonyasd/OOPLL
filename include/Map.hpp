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
        m_Transform.scale = {1.0f, 1.0f};
    }

    const std::vector<std::vector<glm::vec2>>& GetRoutes() const {
        return m_Config.routes;
    }

    // 修正：回傳靜態變數，避免引用到已銷毀的暫存物件
    const std::vector<glm::vec2>& GetRandomRoute() const {
        static const std::vector<glm::vec2> empty_route;
        if (m_Config.routes.empty()) return empty_route;

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