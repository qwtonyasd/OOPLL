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

        // 1. 修改自己的 ZIndex (因為 Tower 繼承自 GameObject，可以直接存取)
        m_ZIndex = 7.0f;

        // 2. 初始化子物件
        m_RangeIndicator = std::make_shared<Util::GameObject>();
        m_RangeIndicator->SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/Start/7.png"));

        // 修正點：使用 SetZIndex 函式而非直接存取變數
        m_RangeIndicator->SetZIndex(5.0f);

        m_UpgradeMenu = std::make_shared<Util::GameObject>();
        m_UpgradeMenu->SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/UI/upgrade_menu.png"));

        // 修正點：使用 SetZIndex 函式
        m_UpgradeMenu->SetZIndex(99.0f);
    }

    virtual ~Tower() = default;

    // 重要：保持 virtual，這樣子類別的 override 才會生效
    virtual void Draw() {
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
            m_UpgradeMenu->m_Transform.translation = m_Transform.translation + glm::vec2(0, 60);
            m_UpgradeMenu->Draw();
        }
    }

    // 更新範圍圈的大小與位置
    void UpdateRangeIndicator() {
        if (m_RangeIndicator) {
            m_RangeIndicator->m_Transform.translation = m_Transform.translation;
            float textureRadius = 125.0f; // 假設圖片半徑
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

    virtual void Attack(std::shared_ptr<Enemy> target,
                        std::vector<std::shared_ptr<Enemy>>& allEnemies,
                        std::vector<std::shared_ptr<Projectile>>& projectiles) = 0;

    virtual void UpdateAnimation() {}

protected:
    std::shared_ptr<Enemy> FindTarget(const std::vector<std::shared_ptr<Enemy>>& enemies) {
        std::shared_ptr<Enemy> closest = nullptr;
        float minDistance = m_Range;
        for (const auto& enemy : enemies) {
            if (!enemy || enemy->GetHP() <= 0) continue;
            float dist = glm::distance(m_Transform.translation, enemy->GetPosition());
            if (dist < minDistance) {
                minDistance = dist;
                closest = enemy;
            }
        }
        return closest;
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