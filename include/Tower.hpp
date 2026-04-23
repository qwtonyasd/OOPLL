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
#include "Util/TransformUtils.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
class Tower : public Util::GameObject {
public:
    enum class Type {
        NONE, ARCHER, BARRACKS, MAGE, BOMB
    };

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
        m_ZIndex = 20.0f;

        // 2. 初始化範圍指示器 (改用 Image)
        m_RangeIndicatorImage = std::make_shared<Util::Image>("../PTSD/assets/sprites/images/Start/6.png");

        // 3. 初始化升級選單 (改用 Image)
        m_UpgradeMenuImage = std::make_shared<Util::Image>("../PTSD/assets/sprites/images/UI/upgrade_menu.png");
    }

    virtual ~Tower() = default;

    // 移除 override，因為報錯顯示父類別可能沒有 virtual Draw() 或簽名不符
    // 我們手動呼叫它來維持本體的繪製
    virtual void Draw() {
        if (m_IsSelected) {
            // --- 畫範圍指示器 ---
            if (m_RangeIndicatorImage) {
                Util::Transform indicatorTransform;
                indicatorTransform.translation = m_Transform.translation;

                // 【縮放公式】：射程 / 圖片原始半徑
                // 假設 6.png 是 200x200 像素，那原始半徑就是 100
                float originalRadius = 125.0f;
                float scale = m_Range / originalRadius;
                indicatorTransform.scale = {scale, scale};

                auto data = Util::ConvertToUniformBufferData(
                    indicatorTransform, m_RangeIndicatorImage->GetSize(), 5.0f);
                m_RangeIndicatorImage->Draw(data);
            }

            // --- 畫升級選單 ---
            if (m_UpgradeMenuImage) {
                Util::Transform menuTransform;
                menuTransform.translation = m_Transform.translation + glm::vec2(0, 70);
                auto data = Util::ConvertToUniformBufferData(
                    menuTransform, m_UpgradeMenuImage->GetSize(), 101.0f);
                m_UpgradeMenuImage->Draw(data);
            }
        }
        // 注意：這裡不要呼叫 Util::GameObject::Draw()，不然會畫出預設的圖
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