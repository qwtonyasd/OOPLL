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
#include "Tower/Projectlie/Projectile.hpp"

#include "Util/GameObject.hpp"
#include "Util/Logger.hpp"

class TowerManager {
public:
    TowerManager(Util::GameObject& root) : m_Root(root) {}

    // --- 新增：處理點擊塔的選取邏輯 ---
    bool HandleClick(const glm::vec2& mousePos) {
        bool hit = false;
        // 我們由後往前遍歷，這樣如果塔有重疊，會優先點到最前面的塔
        for (auto it = m_Towers.rbegin(); it != m_Towers.rend(); ++it) {
            auto& tower = *it;
            if (tower->IsMouseHovering(mousePos)) {
                // 如果點中了這座塔，先清除所有塔的選取，再選中這座
                ClearSelection();
                tower->SetSelected(true);
                hit = true;
                LOG_INFO("Tower Selected at: ({}, {})", tower->GetPosition().x, tower->GetPosition().y);
                break;
            }
        }
        return hit;
    }

    // --- 新增：清除所有塔的選取狀態 (隱藏範圍圈與選單) ---
    void ClearSelection() {
        for (auto& tower : m_Towers) {
            tower->SetSelected(false);
        }
    }

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

    void UpdateAll(std::vector<std::shared_ptr<Enemy>>& enemies,
                   std::vector<std::shared_ptr<Projectile>>& projectiles) {
        for (auto& tower : m_Towers) {
            tower->Update(enemies, projectiles);
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