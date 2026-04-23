#ifndef ARCHER_TOWER_H
#define ARCHER_TOWER_H

#include "Tower.hpp"
#include "Arrow.hpp"
#include "Util/Image.hpp"
#include "Core/Drawable.hpp" // 必須包含這個
#include <vector>
#include <string>

class ArcherTower : public Tower {
public:
    ArcherTower(glm::vec2 pos);

    void Attack(std::shared_ptr<Enemy> target,
                std::vector<std::shared_ptr<Enemy>>& allEnemies,
                std::vector<std::shared_ptr<Projectile>>& projectiles) override;

    void UpdateAnimation() override;
    void Draw() override;

private:
    void FireArrow(glm::vec2 offset);

    std::shared_ptr<Util::Image> m_BaseDrawable;
    std::vector<std::string> m_AttackFramesRight;
    std::vector<std::string> m_AttackFramesLeft;

    // 修正點：改用 Core::Drawable
    std::shared_ptr<Core::Drawable> m_LeftDrawable;
    std::shared_ptr<Core::Drawable> m_RightDrawable;

    float m_LeftAttackStartTime = 0.0f;
    float m_RightAttackStartTime = 0.0f;
    float m_LastAttackTime = 0.0f;

    bool m_IsLeftAttacking = false;
    bool m_IsRightAttacking = false;
    bool m_LeftHasFired = false;
    bool m_RightHasFired = false;
    bool m_NextShooterLeft = true;

    float m_VisualOffset = 18.0f;

    std::shared_ptr<Enemy> m_CurrentTarget;
    std::vector<std::shared_ptr<Projectile>>* m_ProjectilesRef = nullptr;

    // 如果編譯器說找不到 m_AttackRate，請在這邊補上
    float m_AttackRate = 0.8f;
};

#endif