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
#include "Projectile.hpp" // 必須引入 Projectile 基類定義

#include "Util/GameObject.hpp"
#include "Util/Logger.hpp"

class TowerManager {
public:
    // 建構子：接收渲染樹的根節點，用於自動管理渲染
    TowerManager(Util::GameObject& root) : m_Root(root) {}

    /**
     * @brief 在指定位置建造塔
     * @param type 塔的類型 (ARCHER, BARRACKS, 等)
     * @param pos 建造座標
     * @param route 敵人路徑 (主要給兵營使用)
     */
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
            m_Root.AddChild(newTower); // 將塔加入渲染樹
            LOG_INFO("Tower Created: Type {}", static_cast<int>(type));
        }
    }

    /**
     * @brief 更新所有塔的邏輯
     * @param enemies 當前地圖上的敵人列表
     * @param projectiles 用於存放塔射出的箭矢/投射物容器
     */
    void UpdateAll(std::vector<std::shared_ptr<Enemy>>& enemies,
                   std::vector<std::shared_ptr<Projectile>>& projectiles) {
        for (auto& tower : m_Towers) {
            // 呼叫各個塔的 Update，傳入敵人進行目標偵測，並傳入容器來存放產生的箭矢
            tower->Update(enemies, projectiles);
        }
    }

    /**
     * @brief 繪製所有塔 (如果 AddChild 已經處理了渲染，此函式可視情況保留或刪除)
     */
    void DrawAll() {
        for (auto& tower : m_Towers) {
            tower->Draw();
        }
    }

    /**
     * @brief 清理所有塔 (換關卡或重新開始時使用)
     */
    void Clear() {
        for (auto& tower : m_Towers) {
            m_Root.RemoveChild(tower); // 從渲染樹移除
        }
        m_Towers.clear();
    }

private:
    std::vector<std::shared_ptr<Tower>> m_Towers; // 存放所有塔的容器
    Util::GameObject& m_Root; // 渲染樹根節點引用
};

#endif // TOWERMANAGER_HPP