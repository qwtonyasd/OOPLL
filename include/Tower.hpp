#ifndef TOWER_HPP
#define TOWER_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"
#include "Enemy.hpp"
#include "Projectile.hpp"
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <string>
#include <limits> // 用於 float 的無限大判斷

class Tower : public Util::GameObject {
public:
    enum class Type {
        NONE, ARCHER, BARRACKS, MAGE, BOMB
    };

    static int GetBaseCost(Type type) {
        switch (type) {
            case Type::ARCHER:   return 70;
            case Type::BARRACKS: return 70;
            case Type::MAGE:     return 100;
            case Type::BOMB:     return 125;
            default:             return 0;
        }
    }

    Tower(const glm::vec2& pos, const std::string& imgPath, float range, float cooldown, float damage, int cost, Enemy::DamageType damageType = Enemy::DamageType::PHYSICAL)
        : m_Range(range), m_Cooldown(cooldown), m_Damage(damage), m_Cost(cost), m_DamageType(damageType) {
        m_Transform.translation = pos;
        SetDrawable(std::make_shared<Util::Image>(imgPath));

        // 1. 修改自己的 ZIndex (繼承自 GameObject，可直接存取)
        m_ZIndex = 7.0f;

        // 2. 初始化範圍指示器 (Range Indicator)
        m_RangeIndicator = std::make_shared<Util::GameObject>();
        // 注意：請確保路徑下的圖片存在
        m_RangeIndicator->SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/Start/7.png"));
        m_RangeIndicator->SetZIndex(5.0f); // 透過 Setter 修改

        // 3. 初始化升級選單 (Upgrade Menu)
        m_UpgradeMenu = std::make_shared<Util::GameObject>();
        m_UpgradeMenu->SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/UI/upgrade_menu.png"));
        m_UpgradeMenu->SetZIndex(99.0f); // 透過 Setter 修改
    }

    virtual ~Tower() = default;

    // 繪製邏輯
    virtual void Draw()  {
        // 1. 如果選取中，先畫範圍圈
        if (m_IsSelected && m_RangeIndicator) {
            UpdateRangeIndicator();
            m_RangeIndicator->Draw();
        }

        // 2. 畫塔本體 (包含視覺偏移)
        if (m_Drawable) {
            glm::vec2 originalPos = m_Transform.translation;
            m_Transform.translation.y += m_VisualOffset;
            GameObject::Draw();
            m_Transform.translation = originalPos;
        }

        // 3. 如果選取中，最後畫升級選單
        if (m_IsSelected && m_UpgradeMenu) {
            // 將選單位置稍微移到塔上方
            m_UpgradeMenu->m_Transform.translation = m_Transform.translation + glm::vec2(0, 60);
            m_UpgradeMenu->Draw();
        }
    }

    // 更新範圍圈的大小與位置
    void UpdateRangeIndicator() {
        if (m_RangeIndicator) {
            m_RangeIndicator->m_Transform.translation = m_Transform.translation;
            // 假設圖片原始半徑為 125 像素，根據實際 m_Range 進行縮放
            float textureRadius = 125.0f;
            float scale = m_Range / textureRadius;
            m_RangeIndicator->m_Transform.scale = {scale, scale};
        }
    }

    void SetSelected(bool selected) { m_IsSelected = selected; }

    bool IsMouseHovering(const glm::vec2& mousePos) {
        return glm::distance(m_Transform.translation, mousePos) < 40.0f;
    }

    virtual void Update(std::vector<std::shared_ptr<Enemy>>& enemies,
                        std::vector<std::shared_ptr<Projectile>>& projectiles) {
        float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;
        m_Timer += dt;

        if (m_Timer >= m_Cooldown) {
            auto target = FindTarget(enemies);
            if (target) {
                Attack(target, enemies, projectiles);
                m_Timer = 0.0f;
            }
        }
    }

    // 純虛擬函式，由 ArcherTower, MageTower 等子類別實作
    virtual void Attack(std::shared_ptr<Enemy> target,
                        std::vector<std::shared_ptr<Enemy>>& allEnemies,
                        std::vector<std::shared_ptr<Projectile>>& projectiles) = 0;

    virtual void UpdateAnimation() {}

protected:
    // --- 核心邏輯：尋找在射程內且路徑進度最領先的怪 ---
    std::shared_ptr<Enemy> FindTarget(const std::vector<std::shared_ptr<Enemy>>& enemies) {
        std::shared_ptr<Enemy> bestTarget = nullptr;
        float maxProgress = -1.0f; // 我們要找的是累積距離最大的對象

        for (const auto& enemy : enemies) {
            // 基本過濾：排除不存在、已死亡、或已到終點的敵人
            if (!enemy || enemy->GetHP() <= 0 || enemy->ReachedEnd()) continue;

            // 計算塔與敵人間的物理距離
            float dist = glm::distance(m_Transform.translation, enemy->GetPosition());

            // 檢查是否在射程內
            if (dist <= m_Range) {
                // 取得敵人已走過的總路徑長度
                float currentProgress = enemy->GetTotalTravelledDistance();

                // 如果這個敵人的進度領先目前所有的候選者，則更新目標
                if (currentProgress > maxProgress) {
                    maxProgress = currentProgress;
                    bestTarget = enemy;
                }
            }
        }
        return bestTarget;
    }

    float m_Range, m_Cooldown, m_Damage;
    float m_Timer = 0.0f;
    int m_Cost;
    Enemy::DamageType m_DamageType;
    float m_VisualOffset = 0.0f;
    bool m_IsSelected = false;

    std::shared_ptr<Util::GameObject> m_RangeIndicator;
    std::shared_ptr<Util::GameObject> m_UpgradeMenu;
};

#endif