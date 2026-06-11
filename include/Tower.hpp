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
#include "Util/Text.hpp"

class Tower : public Util::GameObject {
public:

    bool IsSellClicked(const glm::vec2& mousePos) {
        if (!m_IsSelected) return false;
        glm::vec2 sellBtnPos = m_Transform.translation + glm::vec2(0.0f, -55.0f);
        return glm::distance(mousePos, sellBtnPos) < 25.0f; // 25.0f 為按鈕判定半徑
    }

    // 🆕 【增加】取得這座塔當前價值的 60% 作為退款
    // 🟢 【修改】改為動態呼叫虛擬函式 GetTotalCost() 來計算 60% 退款
    int GetSellRefund() const {
        return static_cast<int>(GetTotalCost() * 0.6f);
    }

    virtual int GetTotalCost() const {
        return 70;
    }
    virtual bool IsSkillClicked(const glm::vec2& /*mousePos*/) {
        return false;
    }

    virtual int GetClickedSkillIndex(const glm::vec2& /*mousePos*/) {
        return -1;
    }

    virtual void BuySkill(int /*skillIndex*/) {
        // 預設什麼都不做，讓升到 Lv4 的 ArcherTower 去覆寫它
    }

    struct TowerStats {
        float range;
        float attackInterval;
        float damage;
        int upgradeCost;
        std::string baseSpritePath;
    };

    enum class Type {
        NONE, ARCHER, BARRACKS, MAGE, BOMB
    };

    /**
     * @param maxLevel 設定此塔的最高等級 (預設為 3)
     */
    Tower(const glm::vec2& pos, const std::string& imgPath,
          float range, float cooldown, float damage, int cost,
          Enemy::DamageType damageType = Enemy::DamageType::PHYSICAL,
          int maxLevel = 3) // maxLevel 放在最後
        : m_Range(range),
          m_Cooldown(cooldown),
          m_Damage(damage),
          m_Cost(cost),
          m_MaxLevel(maxLevel), // m_MaxLevel 現在是 int，可變動
          m_DamageType(damageType)
    {
        m_Transform.translation = pos;
        SetDrawable(std::make_shared<Util::Image>(imgPath));
        m_ZIndex = 15.0f;

        m_RangeIndicatorImage = std::make_shared<Util::Image>("../PTSD/assets/sprites/images/Start/6.png");
        m_UpgradeMenuImage = std::make_shared<Util::Image>("../PTSD/assets/sprites/images/Start/2.png");
        m_UpgradeIcon = std::make_shared<Util::Image>("../PTSD/assets/sprites/images/Start/20.png");
        m_SellIcon = std::make_shared<Util::Image>("../PTSD/assets/sprites/images/Start/19.png");

        UpdateCostText();
    }

    virtual ~Tower() = default;

    // 介面函數
    virtual void Upgrade() = 0;
    virtual void UpdateAnimation() = 0;
    virtual void Attack(std::shared_ptr<Enemy> target,
                        std::vector<std::shared_ptr<Enemy>>& allEnemies,
                        std::vector<std::shared_ptr<Projectile>>& projectiles) = 0;

    // 基礎邏輯
    bool GetIsSelected() const { return m_IsSelected; }
    void SetSelected(bool selected) { m_IsSelected = selected; }
    glm::vec2 GetPosition() const { return m_Transform.translation; }
    int GetUpgradeCost() const { return m_Cost; }

    bool IsUpgradeClicked(const glm::vec2& mousePos) {
        if (!m_IsSelected || m_Level >= m_MaxLevel) return false;
        glm::vec2 upgradeBtnPos = m_Transform.translation + glm::vec2(0.0f, 60.0f);
        return glm::distance(mousePos, upgradeBtnPos) < 25.0f;
    }

    bool IsMouseHovering(const glm::vec2& mousePos) {
        return glm::distance(m_Transform.translation, mousePos) < 40.0f;
    }

    virtual void Draw()  {
        if (!m_IsSelected) return;

        if (m_RangeIndicatorImage) {
            float scale = m_Range / 125.0f;
            m_RangeIndicatorImage->Draw(Util::ConvertToUniformBufferData(
                Util::Transform{m_Transform.translation, 0, {scale, scale}},
                m_RangeIndicatorImage->GetSize(), 5.0f));
        }

        if (m_UpgradeMenuImage) {
            m_UpgradeMenuImage->Draw(Util::ConvertToUniformBufferData(
                Util::Transform{m_Transform.translation + glm::vec2(0, 5), 0, {1.1f, 1.1f}},
                m_UpgradeMenuImage->GetSize(), 16.0f));
        }

        if (m_UpgradeIcon && m_Level < m_MaxLevel) {
            m_UpgradeIcon->Draw(Util::ConvertToUniformBufferData(
                Util::Transform{m_Transform.translation + glm::vec2(0.0f, 60.0f), 0, {0.8f, 0.8f}},
                m_UpgradeIcon->GetSize(), 18.0f));
        }

        if (m_SellIcon) {
            m_SellIcon->Draw(Util::ConvertToUniformBufferData(
                Util::Transform{m_Transform.translation + glm::vec2(0.0f, -55.0f), 0, {0.8f, 0.8f}},
                m_SellIcon->GetSize(), 18.0f));
        }

        if (m_CostText && m_CostText->GetSize().x > 0) {
            m_CostText->Draw(Util::ConvertToUniformBufferData(
                Util::Transform{m_Transform.translation + glm::vec2(5.0f, 40.0f), 0, {0.5f, 0.5f}},
                m_CostText->GetSize(), 19.0f));
        }
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

    static int GetBaseCost(Type type) {
        switch (type) {
            case Type::ARCHER: return 70;
            case Type::BARRACKS: return 70;
            case Type::MAGE: return 100;
            case Type::BOMB: return 125;
            default: return 0;
        }
    }

protected:
    void UpdateCostText() {
        std::string displayStr = (m_Level < m_MaxLevel) ? std::to_string(m_Cost) : "MAX";
        m_CostText = std::make_shared<Util::Text>(m_FontPath, 20, displayStr, Util::Color::FromName(Util::Colors::YELLOW));
    }

    void ApplyBaseStats(const TowerStats& stats) {
        m_Range = stats.range;
        m_Cooldown = stats.attackInterval;
        m_Damage = stats.damage;
        m_Cost = stats.upgradeCost;
        SetDrawable(std::make_shared<Util::Image>(stats.baseSpritePath));
        UpdateCostText();
    }

    virtual std::shared_ptr<Enemy> FindTarget(const std::vector<std::shared_ptr<Enemy>>& enemies) {
        std::shared_ptr<Enemy> bestTarget = nullptr;
        std::shared_ptr<Enemy> secondaryTarget = nullptr;

        float maxProgress = -1.0f;
        float maxProgressNoRoot = -1.0f;

        for (const auto& enemy : enemies) {
            if (!enemy || enemy->GetHP() <= 0 || enemy->ReachedEnd()) continue;

            float dist = glm::distance(m_Transform.translation, enemy->GetPosition());
            if (dist <= m_Range) {
                float progress = enemy->GetTotalTravelledDistance();

                // 1. 記錄所有怪物中進度最遠的 (作為備選方案)
                if (progress > maxProgress) {
                    maxProgress = progress;
                    bestTarget = enemy;
                }

                // 2. 記錄「未被定身」的怪物中進度最遠的 (作為優先方案)
                // 假設你的 Enemy 類別有一個 IsRooted() 函式 (請參閱下方說明)
                if (!enemy->IsRooted() && progress > maxProgressNoRoot) {
                    maxProgressNoRoot = progress;
                    secondaryTarget = enemy;
                }
            }
        }

        // 優先回傳未被定身的怪物，如果沒得選，才回傳被定身的怪物
        return (secondaryTarget) ? secondaryTarget : bestTarget;
    }

    float m_Range, m_Cooldown, m_Damage;
    float m_Timer = 0.0f;
    int m_Cost;
    int m_TotalSpent = 0; // 🆕 【增加這一行】追蹤此塔累積花費（初始費 + 歷次升級費）
    int m_Level = 1;
    int m_MaxLevel; // 已移除 const，現在可在建構子中賦值

    Enemy::DamageType m_DamageType;
    float m_VisualOffset = 0.0f;
    bool m_IsSelected = false;
    std::string m_FontPath = "../PTSD/assets/sprites/images/fonts/7_Comic Book.ttf";

    std::shared_ptr<Util::Text> m_CostText;
    std::shared_ptr<Util::Image> m_UpgradeIcon;
    std::shared_ptr<Util::Image> m_SellIcon;
    std::shared_ptr<Util::Image> m_RangeIndicatorImage;
    std::shared_ptr<Util::Image> m_UpgradeMenuImage;
};

#endif