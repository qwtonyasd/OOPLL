#ifndef MAP_HPP
#define MAP_HPP

#include <vector>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Enemy.hpp"

/**
 * @brief 定義特定路徑要出的怪與延遲
 * 在 App::HandleGamePlay 中會根據 spawnDelay 來決定出怪時機
 */
struct SubWave {
    int routeIndex;                   // 指定路徑索引
    std::vector<Enemy::Type> enemies; // 怪物清單 (通常是一隻)
    float spawnDelay;                 // 出現延遲時間 (相對於波次開始)

    // 建構子，方便在 emplace_back 使用
    SubWave(int route, std::vector<Enemy::Type> types, float delay)
        : routeIndex(route), enemies(types), spawnDelay(delay) {}
};

/**
 * @brief 一個波次包含多個子波次行為
 */
struct WaveConfig {
    std::vector<SubWave> subWaves;
};

/**
 * @brief 關卡配置結構
 */
struct LevelConfig {
    int levelId;
    int initialMoney;
    std::string imagePath;
    std::vector<std::vector<glm::vec2>> routes;
    std::vector<glm::vec2> towerSlotPositions;
    std::vector<WaveConfig> waves;
};

class Map : public Util::GameObject {
public:
    explicit Map(const LevelConfig& config) : m_Config(config) {
        m_Drawable = std::make_shared<Util::Image>(config.imagePath);
        m_ZIndex = -10.0f;
    }

    // 取得所有路徑
    const std::vector<std::vector<glm::vec2>>& GetRoutes() const { return m_Config.routes; }

    // 根據索引取得路徑，若索引錯誤則回傳隨機路徑
    const std::vector<glm::vec2>& GetRouteByIndex(int index) const {
        if (index >= 0 && index < static_cast<int>(m_Config.routes.size())) {
            return m_Config.routes[index];
        }
        return GetRandomRoute();
    }

    // 隨機取得一條路徑
    const std::vector<glm::vec2>& GetRandomRoute() const {
        if (m_Config.routes.empty()) {
            static const std::vector<glm::vec2> empty;
            return empty;
        }
        return m_Config.routes[rand() % m_Config.routes.size()];
    }

    // 取得塔位座標清單
    const std::vector<glm::vec2>& GetTowerSlots() const { return m_Config.towerSlotPositions; }

    // 取得所有波次配置
    const std::vector<WaveConfig>& GetWaves() const { return m_Config.waves; }

    // 取得初始金錢
    int GetInitialMoney() const { return m_Config.initialMoney; }

private:
    LevelConfig m_Config;
};

#endif