#ifndef TOWER_HPP
#define TOWER_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"
#include "Enemy.hpp"
#include "Tower/Projectile/Projectile.hpp"
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Util/Logger.hpp"
#include <string>
#include "Util/TransformUtils.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
class Tower : public Util::GameObject {
public:
    struct TowerStats {
        float range;
        float attackInterval;
        float damage;
        int upgradeCost;
        std::string baseSpritePath; // 所有的塔都有基座
    };
    enum class Type {
        NONE, ARCHER, BARRACKS, MAGE, BOMB
    };
    virtual void Upgrade() = 0;
    glm::vec2 GetPosition() const {
        return m_Transform.translation;
    }

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

        // 1. 設定塔本身的層級
        m_ZIndex = 15.0f;

        // 2. 初始化範圍指示器 (改用 Image)
        m_RangeIndicatorImage = std::make_shared<Util::Image>("../PTSD/assets/sprites/images/Start/6.png");

        // 3. 初始化升級選單 (改用 Image)
        m_UpgradeMenuImage = std::make_shared<Util::Image>("../PTSD/assets/sprites/images/Start/2.png");
        if (m_UpgradeMenuImage->GetSize().x == 0) {
            LOG_DEBUG("Failed to load Upgrade Menu Image! Check path.");
        }
    }

    virtual ~Tower() = default;

    // 移除 override，因為報錯顯示父類別可能沒有 virtual Draw() 或簽名不符
    // 我們手動呼叫它來維持本體的繪製
    virtual void Draw() {
        if (m_IsSelected) {
            // --- 1. 繪製範圍指示器 (藍圈) ---
            // 這個縮放是正確的，因為範圍圈必須跟隨 m_Range
            if (m_RangeIndicatorImage) {
                Util::Transform indicatorTransform;
                indicatorTransform.translation = m_Transform.translation;

                float originalRadius = 125.0f;
                float scale = m_Range / originalRadius;
                indicatorTransform.scale = {scale, scale}; // 這裡會變動

                auto indicatorData = Util::ConvertToUniformBufferData(
                    indicatorTransform,
                    m_RangeIndicatorImage->GetSize(),
                    5.0f // 最底層
                );
                m_RangeIndicatorImage->Draw(indicatorData);
            }

            // --- 2. 繪製升級選單 (UI 圓環) ---
            // 注意：這裡必須使用獨立的 Transform 且 scale 設為固定值
            if (m_UpgradeMenuImage) {
                Util::Transform menuTransform;
                menuTransform.translation = m_Transform.translation + glm::vec2(0.0f, 5.0f);;

                // 強制設為 1.0f，不使用任何與 m_Range 相關的變數
                menuTransform.scale = {1.1, 1.1f};

                auto menuData = Util::ConvertToUniformBufferData(
                    menuTransform,
                    m_UpgradeMenuImage->GetSize(),
                    16.0f // 提高 ZIndex 確保在最前方
                );
                m_UpgradeMenuImage->Draw(menuData);
            }
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
    int m_Level = 1;
    const int m_MaxLevel = 3;

    // 通用數值應用：由子類別呼叫
    void ApplyBaseStats(const TowerStats& stats) {
        m_Range = stats.range;
        m_Cooldown = stats.attackInterval; // 對應你原本的 m_Cooldown
        m_Damage = stats.damage;
        m_Cost = stats.upgradeCost;
        SetDrawable(std::make_shared<Util::Image>(stats.baseSpritePath));
    }
    std::shared_ptr<Enemy> FindTarget(const std::vector<std::shared_ptr<Enemy>>& enemies) {
        std::shared_ptr<Enemy> bestTarget = nullptr;
        float maxProgress = -1.0f;

        for (const auto& enemy : enemies) {
            if (!enemy || enemy->GetHP() <= 0 || enemy->ReachedEnd()) continue;

            float dist = glm::distance(m_Transform.translation, enemy->GetPosition());
            if (dist <= m_Range) {
                float currentProgress = enemy->GetTotalTravelledDistance();
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

    // 直接儲存 Image 指標，避開 GameObject 的權限限制
    std::shared_ptr<Util::Image> m_RangeIndicatorImage;
    std::shared_ptr<Util::Image> m_UpgradeMenuImage;
};

#endif