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
#include "Util/Text.hpp"
class Tower : public Util::GameObject {
public:
    // 加入這個函數，讓 App 可以檢查塔是否被選中
    bool GetIsSelected() const { return m_IsSelected; }

    // 順便確認是否有 IsUpgradeClicked
    bool IsUpgradeClicked(const glm::vec2& mousePos) const {
        if (!m_IsSelected || m_Level >= m_MaxLevel) return false;
        glm::vec2 upgradeBtnPos = m_Transform.translation + glm::vec2(0.0f, 50.0f);
        return glm::distance(mousePos, upgradeBtnPos) < 25.0f;
    }
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
        // 增加升級與出售圖示 (請確保路徑正確)
        m_UpgradeIcon = std::make_shared<Util::Image>("../PTSD/assets/sprites/images/Start/20.png");
        m_SellIcon = std::make_shared<Util::Image>("../PTSD/assets/sprites/images/Start/19.png");
    }

    virtual ~Tower() = default;
    //檢查有沒有點到升級塔
    bool IsUpgradeClicked(const glm::vec2& mousePos) {
        if (!m_IsSelected || m_Level >= m_MaxLevel) return false;

        // 計算升級按鈕的中心位置 (需與 Draw() 裡的位移一致)
        glm::vec2 upgradeBtnPos = m_Transform.translation + glm::vec2(0.0f, 60.0f);

        // 簡單的圓形或方形碰撞檢查 (假設按鈕大小約 40x40)
        return glm::distance(mousePos, upgradeBtnPos) < 25.0f;
    }

    // 取得當前升級所需費用
    int GetUpgradeCost() const { return m_Cost; }

    // 移除 override，因為報錯顯示父類別可能沒有 virtual Draw() 或簽名不符
    // 我們手動呼叫它來維持本體的繪製
    virtual void Draw() {
        if (!m_IsSelected) return;

        // 1. 範圍圈 (底層)
        if (m_RangeIndicatorImage) {
            float scale = m_Range / 125.0f;
            m_RangeIndicatorImage->Draw(Util::ConvertToUniformBufferData(
                Util::Transform{m_Transform.translation, 0, {scale, scale}},
                m_RangeIndicatorImage->GetSize(), 5.0f));
        }

        // 2. 升級選單圓環 (中層)
        if (m_UpgradeMenuImage) {
            m_UpgradeMenuImage->Draw(Util::ConvertToUniformBufferData(
                Util::Transform{m_Transform.translation + glm::vec2(0, 5), 0, {1.1f, 1.1f}},
                m_UpgradeMenuImage->GetSize(), 16.0f));
        }

        // 3. 升級標誌 (上方)
        if (m_UpgradeIcon && m_Level < m_MaxLevel) {
            m_UpgradeIcon->Draw(Util::ConvertToUniformBufferData(
                Util::Transform{m_Transform.translation + glm::vec2(0.0f, 60.0f), 0, {0.8f, 0.8f}},
                m_UpgradeIcon->GetSize(), 18.0f));
        }

        // 4. 出售標誌 (下方)
        if (m_SellIcon) {
            m_SellIcon->Draw(Util::ConvertToUniformBufferData(
                Util::Transform{m_Transform.translation + glm::vec2(0.0f, -55.0f), 0, {0.8f, 0.8f}},
                m_SellIcon->GetSize(), 18.0f));
        }

        // 5. 價格文字 (最頂層)
        if (m_CostText != nullptr) {
            auto textSize = m_CostText->GetSize();
            if (textSize.x > 0) {
                m_CostText->Draw(Util::ConvertToUniformBufferData(
                    Util::Transform{m_Transform.translation + glm::vec2(5.0f, 40.0f), 0, {0.5f, 0.5f}},
                    textSize, 19.0f));
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
    std::shared_ptr<Util::Text> m_CostText;
    std::string m_FontPath = "../PTSD/assets/sprites/images/fonts/7_Comic Book.ttf"; // 改成你的路徑
    std::shared_ptr<Util::Image> m_UpgradeIcon;
    std::shared_ptr<Util::Image> m_SellIcon;
    int m_Level = 1;
    const int m_MaxLevel = 3;

    void UpdateCostText() {
        std::string displayStr = (m_Level < m_MaxLevel) ? std::to_string(m_Cost) : "MAX";
        // 建立文字物件：路徑, 字體大小, 內容, 顏色
        m_CostText = std::make_shared<Util::Text>(m_FontPath, 20, displayStr, Util::Color::FromName(Util::Colors::YELLOW));
    }
    // 通用數值應用：由子類別呼叫
    void ApplyBaseStats(const TowerStats& stats) {
        m_Range = stats.range;
        m_Cooldown = stats.attackInterval;
        m_Damage = stats.damage;
        m_Cost = stats.upgradeCost;
        SetDrawable(std::make_shared<Util::Image>(stats.baseSpritePath));

        UpdateCostText(); // 數值改變時同步更新文字內容
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