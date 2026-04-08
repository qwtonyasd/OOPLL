#ifndef SOLDIER_HPP
#define SOLDIER_HPP

#include "Unit.hpp"
#include "Enemy.hpp"
#include <vector>
#include <string>

enum class SoldierState { MOVE, SEARCH, CHASE, ATTACK, DEAD };

class Soldier : public Unit {
public:
    // 統一建構子參數 (4個)
    Soldier(glm::vec2 spawnPos, glm::vec2 targetPos, float speed, float hp);

    // 必須覆寫父類別的純虛擬函式，不能加參數
    void Update() override;

    // 這是你真正用來處理戰鬥的邏輯
    void UpdateLogic(std::vector<std::shared_ptr<Enemy>>& enemies);

    // 如果父類別 Unit 沒有 Draw()，請移除 override
    // 如果父類別有，就保留。這裡假設父類別是 GameObject 有 Draw()
    void Draw();

    float GetHP() const { return m_HP; }
    bool IsDeadAnimationFinished() const { return m_IsDeadAnimDone; }

private:
    void UpdateAnimation(float dt);
    void MoveTo(glm::vec2 target);

    glm::vec2 m_RallyPoint;
    std::shared_ptr<Enemy> m_TargetEnemy = nullptr;
    SoldierState m_CurrentState = SoldierState::MOVE;

    float m_AnimTimer = 0.0f;
    int m_AnimIndex = 0;
    bool m_IsDeadAnimDone = false;

    std::vector<std::string> m_WalkPaths;
    std::vector<std::string> m_AttackPaths;
    std::vector<std::string> m_DeadPaths;
};

#endif