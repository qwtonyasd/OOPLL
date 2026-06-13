#pragma once
#include "Unit.hpp"
#include "Enemy.hpp"
#include "Util/Image.hpp"
#include "Util/Animation.hpp"
#include <vector>
#include <memory>
#include <string>

class Soldier : public Unit, public std::enable_shared_from_this<Soldier> {
public:
    struct SoldierConfig {
        std::string spriteRootPath;
        int walkStart, walkEnd;
        int atkStart, atkEnd;
        int deadStart, deadEnd;
        float maxHP;
        float speed;
        int minDamage;
        int maxDamage;
        float attackCooldown;
        float detectionRange;
        float meleeRange;
    };

    enum class State { IDLE, MOVE_TO_RALLY, CHASE, BLOCKING, DEATH };

    Soldier(glm::vec2 spawnPos, glm::vec2 rallyPoint, const SoldierConfig& config);

    void Update(std::vector<std::shared_ptr<Enemy>>& enemies, float dt) override;
    void Draw() override;

    void EngageTarget(std::shared_ptr<Enemy> enemy);
    void ReleaseEnemy();
    void SetState(State newState);
    bool IsAvailable() const;
    void TakeDamage(float damage);
    void Upgrade(const SoldierConfig& newConfig);

    bool IsDeadAnimationFinished() const { return m_IsDeadAnimDone; }

    // 🎯 修正：回傳 shared_ptr，內部透過 lock() 轉型
    std::shared_ptr<Enemy> GetTargetEnemy() const { return m_TargetEnemy.lock(); }

    // 🎯 修正：使用 lock() 檢查
    bool IsInCombat() const {
        return (m_CurrentState == State::CHASE || m_CurrentState == State::BLOCKING) && !m_TargetEnemy.expired();
    }

protected:
    void MoveTowardsRallyPoint(float dt);
    void ChaseEnemy(float dt);
    void PerformAttack(float dt);
    void UpdateAnimation(float dt);
    void UpdateIdleRotation(float dt);
    void SearchForEnemy(std::vector<std::shared_ptr<Enemy>>& enemies);

protected:
    SoldierConfig m_Config;
    State m_CurrentState = State::IDLE;
    glm::vec2 m_RallyPoint;

    // 🎯 核心修正：改用 weak_ptr，防止強引用卡死生命週期與髒指針
    std::weak_ptr<Enemy> m_TargetEnemy;

    std::shared_ptr<Util::Animation> m_DeadAni;

    float m_AnimTimer = 0.0f;
    float m_AttackTimer = 0.0f;
    int m_FrameIndex = 0;
    bool m_IsMainBlocker = false;
    bool m_IsDeadAnimDone = false;

    float m_TurnTimer = 0.0f;
    float m_NextTurnTime = 3.0f;
    bool m_FacingRight = true;
    float m_ReScanTimer = 0.0f;
};