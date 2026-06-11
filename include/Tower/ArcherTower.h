#ifndef ARCHER_TOWER_H
#define ARCHER_TOWER_H

#include "Tower.hpp"
#include "Tower/Projectile/Projectile.hpp"
#include "Util/Image.hpp"
#include "Core/Drawable.hpp"
#include "Util/Logger.hpp"
#include <vector>
#include <string>
#include "GameData.hpp"

class ArcherTower : public Tower {
public:
    int GetTotalCost() const override {
        // 根據當前等級（m_Level），精確回傳該階段累積的總花費
        switch (m_Level) {
            case 1: return 70;                  // 1等造價
            case 2: return 70 + 110;            // 1等 + 2等升級費 (180)
            case 3: return 70 + 110 + 160;      // 1等 + 2等 + 3等升級費 (340)
            case 4: return 250;                 // 🎯 你提到四等總耗是 250
            default: return 70;
        }
    }


    // 🆕 【新增】全域天賦加成計算函式宣告
    void ApplyGlobalUpgrades();

    static constexpr int SKILL_A_COST = 10;
    static constexpr int SKILL_B_COST = 10;
    bool IsSkillClicked(const glm::vec2& mousePos);
    std::shared_ptr<Enemy> FindTarget(const std::vector<std::shared_ptr<Enemy>>& enemies) override;
    // 新增此宣告
    int GetClickedSkillIndex(const glm::vec2& mousePos);

    // 購買技能函數
    void BuySkill(int skillIndex);

    ArcherTower(glm::vec2 pos);
    void Attack(std::shared_ptr<Enemy> target,
                std::vector<std::shared_ptr<Enemy>>& allEnemies,
                std::vector<std::shared_ptr<Projectile>>& projectiles) override;
    void UpdateAnimation() override;
    void Draw() override;
    void Upgrade() override;
    void UpdateSkillText();
    void Update(std::vector<std::shared_ptr<Enemy>>& enemies,
            std::vector<std::shared_ptr<Projectile>>& projectiles) override;

private:
    TowerStats m_BaseStats;
    float m_SkillBCooldown = 7.0f;
    float m_LastSkillBTime = -10.0f; // 初始設為負數
    void TryAutoCastSkillB(std::vector<std::shared_ptr<Enemy>>& enemies);

    std::shared_ptr<Util::Text> m_SkillTextA;
    std::shared_ptr<Util::Text> m_SkillTextB;

    bool m_SkillA_Learned = false;
    bool m_SkillB_Learned = false;

    std::shared_ptr<Util::Image> m_SkillIcon1;
    std::shared_ptr<Util::Image> m_SkillIcon2;
    void LoadSkillIcons();

    void LoadLevelAssets();
    std::vector<TowerStats> m_ArcherStats;

    std::vector<std::string> m_UpFrames;
    std::vector<std::string> m_DownFrames;

    std::shared_ptr<Core::Drawable> m_LeftDrawable;
    std::shared_ptr<Core::Drawable> m_RightDrawable;

    float m_LeftAttackStartTime = 0.0f, m_RightAttackStartTime = 0.0f;
    bool m_IsLeftAttacking = false, m_IsRightAttacking = false;
    bool m_LeftHasFired = false, m_RightHasFired = false;
    bool m_NextShooterLeft = true;
    bool m_LeftShootUp = false;
    bool m_RightShootUp = false;

    std::shared_ptr<Enemy> m_CurrentTarget;
    std::vector<std::shared_ptr<Projectile>>* m_ProjectilesRef = nullptr;
};

#endif

