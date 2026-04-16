#ifndef TOWERMANAGER_HPP
#define TOWERMANAGER_HPP

#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "Tower.hpp"
#include "tower/Barracks.h"
#include "tower/ArcherTower.h"
#include "tower/MageTower.h"
#include "tower/BombTower.h"
#include "Projectile.hpp"

#include "Util/GameObject.hpp"
#include "Util/Logger.hpp"

class TowerManager {
public:
    TowerManager(Util::GameObject& root) : m_Root(root) {}

    void AddTower(Tower::Type type, glm::vec2 pos, const std::vector<glm::vec2>& route) {
        std::shared_ptr<Tower> newTower = nullptr;

        switch (type) {
            case Tower::Type::ARCHER:
                newTower = std::make_shared<ArcherTower>(pos);
                break;
            case Tower::Type::BARRACKS:
                newTower = std::make_shared<Barracks>(pos, route);
                break;
            case Tower::Type::MAGE:
                newTower = std::make_shared<MageTower>(pos);
                break;
            case Tower::Type::BOMB:
                newTower = std::make_shared<BombTower>(pos);
                break;
            default:
                LOG_ERROR("Unknown Tower Type!");
                return;
        }

        if (newTower) {
            m_Towers.push_back(newTower);
            m_Root.AddChild(newTower);
            LOG_INFO("Tower Created: Type {}", static_cast<int>(type));
        }
    }

    /**
     * @brief 更新所有塔的邏輯與動畫
     */
    void UpdateAll(std::vector<std::shared_ptr<Enemy>>& enemies,
                   std::vector<std::shared_ptr<Projectile>>& projectiles) {
        for (auto& tower : m_Towers) {
            // 1. 執行原本的攻擊邏輯（偵測敵人、冷卻計時、產生子彈）
            tower->Update(enemies, projectiles);

            // 2. 新增：執行塔的動畫更新（處理圖片切換）
            // 只要是在子類別有實作 UpdateAnimation 的塔，都會在這裡動起來
            tower->UpdateAnimation();
        }
    }

    void DrawAll() {
        for (auto& tower : m_Towers) {
            tower->Draw();
        }
    }

    void Clear() {
        for (auto& tower : m_Towers) {
            m_Root.RemoveChild(tower);
        }
        m_Towers.clear();
    }

private:
    std::vector<std::shared_ptr<Tower>> m_Towers;
    Util::GameObject& m_Root;
};

#endif