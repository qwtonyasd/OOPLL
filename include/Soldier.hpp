#ifndef SOLDIER_HPP
#define SOLDIER_HPP

#include "Unit.hpp"
#include "Enemy.hpp"
#include <vector>
#include <memory>

class Soldier : public Unit, public std::enable_shared_from_this<Soldier> {
public:
    enum class State { MOVE_TO_RALLY, IDLE, CHASE, BLOCKING, DEATH };

    Soldier(glm::vec2 spawnPos, glm::vec2 rallyPoint, float speed, float hp = 50.0f);

    void Update() override {}
    void Update(std::vector<std::shared_ptr<Enemy>>& enemies);

    void TakeDamage(float damage);
    float GetHP() const { return m_HP; }
    bool IsDeadAnimationFinished() const { return m_IsDeadAnimDone; }
    void ReleaseEnemy();
    void SetState(State newState);

    // --- 指揮官介面 ---
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

    float m_DetectionRange = 120.0f;
    float m_MeleeRange = 25.0f;
};

#endif