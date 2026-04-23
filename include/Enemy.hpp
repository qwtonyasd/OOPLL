#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Unit.hpp"
#include "Util/Animation.hpp"
#include <vector>
#include <string>
#include <memory>
#include <cmath>

// 向前宣告，避免 Soldier.hpp 與 Enemy.hpp 互相包含導致編譯失敗
class Soldier;

class Enemy : public Unit {
public:
    enum class Type { GOBLIN, ORC };
    enum class State { MOVE_RIGHT, MOVE_UP, MOVE_DOWN, ATTACK, DEATH };
    enum class DamageType { PHYSICAL, MAGIC };

    Enemy(Enemy::Type type, const std::vector<glm::vec2>& path, float speed, float hp,
          const std::vector<std::vector<std::string>>& moveAnimations,
          const std::vector<std::string>& attackPaths,
          const std::vector<std::string>& deadPaths);

    void Update() override;

    void TakeDamage(float damage, DamageType damageType = DamageType::PHYSICAL);
    void UpdateDirection(glm::vec2 dir);
    void SetState(State newState);
    void OnDeath();

    bool IsDeadAnimationFinished() const;

    Enemy::Type GetType() const { return m_Type; }
    glm::vec2 GetPosition() const { return m_Transform.translation; }

    // 修正：當設定為 Blocked 時，需傳入是哪個士兵擋住它
    void SetBlocked(bool b, std::shared_ptr<Soldier> soldier = nullptr) {
        m_IsBlocked = b;
        m_TargetSoldier = soldier;
    }

    bool IsBlocked() const { return m_IsBlocked; }
    float GetHP() const { return m_HP; }
    bool ReachedEnd() const { return m_ReachedEnd; }
    float GetTotalTravelledDistance() const { return m_TotalDistanceTravelled; }

private:
    Enemy::Type m_Type;
    State m_CurrentState;
    bool m_IsBlocked = false;
    bool m_ReachedEnd = false;
    float m_TotalDistanceTravelled = 0.0f;

    // 還擊邏輯變數
    std::shared_ptr<Soldier> m_TargetSoldier = nullptr;
    float m_AttackTimer = 0.0f;
    float m_AttackCooldown = 1.0f; // 1秒打一下

    std::shared_ptr<Util::Animation> m_MoveRightAni, m_MoveUpAni, m_MoveDownAni, m_AttackAni, m_DeadAni;
};

#endif