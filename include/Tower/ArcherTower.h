#ifndef ARCHER_TOWER_H
#define ARCHER_TOWER_H

#include "Tower.hpp"
#include "Tower/Projectile/Projectile.hpp"
#include "Util/Image.hpp"
#include "Core/Drawable.hpp"
#include "Util/Logger.hpp"
#include <vector>
#include <string>

class ArcherTower : public Tower {
public:
    static constexpr int SKILL_A_COST = 100;
    static constexpr int SKILL_B_COST = 100;
    bool IsSkillClicked(const glm::vec2& mousePos);

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

private:
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

