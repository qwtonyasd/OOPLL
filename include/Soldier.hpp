#ifndef SOLDIER_HPP
#define SOLDIER_HPP

#include "Unit.hpp"
#include "Enemy.hpp"
#include <vector>
#include <memory>

class Soldier : public Unit {
public:
    enum class State { MOVE_TO_RALLY, IDLE, BLOCKING, DEATH };

    Soldier(glm::vec2 spawnPos, glm::vec2 rallyPoint, float speed, float hp);

    void Update() override {}
    void Update(std::vector<std::shared_ptr<Enemy>>& enemies);

    float GetHP() const { return m_HP; }
    bool IsDeadAnimationFinished() const { return m_IsDeadAnimDone; }
    void ReleaseEnemy();
    void SetState(State newState);

private:
    void UpdateAnimation(float dt);
    void MoveTowardsRallyPoint();
    void SearchForEnemy(std::vector<std::shared_ptr<Enemy>>& enemies);
    void PerformAttack();

    State m_CurrentState = State::MOVE_TO_RALLY;
    glm::vec2 m_RallyPoint;
    std::shared_ptr<Enemy> m_TargetEnemy = nullptr;

    float m_AnimTimer = 0.0f;
    int m_FrameIndex = 1;
    bool m_IsDeadAnimDone = false;
    float m_AttackDamage = 20.0f; // 每秒造成的傷害
};

#endif