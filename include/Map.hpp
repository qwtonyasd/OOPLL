#ifndef MAP_HPP
#define MAP_HPP

#include <vector>
#include <string>
#include <memory>
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Enemy.hpp" // 需要 Enemy::Type

// 定義每一波的內容
struct WaveConfig {
    std::vector<Enemy::Type> enemyList;
};

// 完整關卡配置
struct LevelConfig {
    int levelId;
    int initialMoney;
    std::string imagePath;
    std::vector<std::vector<glm::vec2>> routes;
    std::vector<glm::vec2> towerSlotPositions;
    std::vector<WaveConfig> waves; // 新增：波次資訊
};

class Map : public Util::GameObject {
public:
    explicit Map(const LevelConfig& config) : m_Config(config) {
        m_Drawable = std::make_shared<Util::Image>(config.imagePath);
        m_ZIndex = -10.0f;
    }

    const std::vector<std::vector<glm::vec2>>& GetRoutes() const { return m_Config.routes; }

    const std::vector<glm::vec2>& GetRandomRoute() const {
        if (m_Config.routes.empty()) {
            static const std::vector<glm::vec2> empty;
            return empty;
        }
        return m_Config.routes[rand() % m_Config.routes.size()];
    }

    const std::vector<glm::vec2>& GetTowerSlots() const { return m_Config.towerSlotPositions; }

    // 新增：取得波次資料
    const std::vector<WaveConfig>& GetWaves() const { return m_Config.waves; }
    int GetInitialMoney() const { return m_Config.initialMoney; }

private:
    LevelConfig m_Config;
};

#endif