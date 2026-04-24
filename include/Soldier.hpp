#ifndef SOLDIER_HPP
#define SOLDIER_HPP

#include "Unit.hpp"
#include <vector>
#include <memory>

// 前向宣告 Enemy
class Enemy;

class Soldier : public Unit, public std::enable_shared_from_this<Soldier> {
public:
    enum class State { MOVE_TO_RALLY, IDLE, CHASE, BLOCKING, DEATH };

    Soldier(glm::vec2 spawnPos, glm::vec2 rallyPoint, float speed, float hp = 50.0f);

    // 修正：確保 Draw 與 Update 都有 override 宣告
    void Draw() override;
    void Update() override {}
    void Update(std::vector<std::shared_ptr<Enemy>>& enemies);

    void TakeDamage(float damage);
    bool IsDeadAnimationFinished() const { return m_IsDeadAnimDone; }
    void ReleaseEnemy();
    void SetState(State newState);

    void EngageTarget(std::shared_ptr<Enemy> enemy);
    bool IsAvailable() const;

private:
    void UpdateAnimation(float dt);
    void MoveTowardsRallyPoint();
    void SearchForEnemy(std::vector<std::shared_ptr<Enemy>>& enemies);
    void ChaseEnemy(float dt);
    void PerformAttack(float dt);
    void UpdateIdleRotation(float dt);

    State m_CurrentState = State::MOVE_TO_RALLY;
    glm::vec2 m_RallyPoint;
    std::shared_ptr<Enemy> m_TargetEnemy = nullptr;

    float m_AnimTimer = 0.0f;
    int m_FrameIndex = 1;
    bool m_IsDeadAnimDone = false;
    float m_AttackTimer = 0.0f;
    float m_AttackCooldown = 1.0f;
    float m_TurnTimer = 0.0f;
    float m_NextTurnTime = 0.0f;

    float m_DetectionRange = 80.0f;
    float m_MeleeRange = 20.0f;
};

#endif