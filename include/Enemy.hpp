#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Unit.hpp"
#include "Util/Animation.hpp"
#include <vector>
#include <string>
#include <memory>

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
    void Draw() override;

    void TakeDamage(float damage, DamageType damageType = DamageType::PHYSICAL);
    void UpdateDirection(glm::vec2 dir);
    void SetState(State newState);
    void OnDeath();
    bool IsDeadAnimationFinished() const;

    // --- 新增的公開存取介面 (解決編譯錯誤) ---
    bool ReachedEnd() const { return m_ReachedEnd; }
    float GetTotalTravelledDistance() const { return m_TotalDistanceTravelled; }
    Enemy::Type GetType() const { return m_Type; }
    // ------------------------------------

    glm::vec2 GetPosition() const { return m_Transform.translation; }
    void SetBlocked(bool b, std::shared_ptr<Soldier> soldier = nullptr) {
        m_IsBlocked = b;
        m_TargetSoldier = soldier;
    }

private:
    Enemy::Type m_Type;
    State m_CurrentState;
    bool m_IsBlocked = false;
    bool m_ReachedEnd = false; // 原本外部無法存取
    float m_TotalDistanceTravelled = 0.0f; // 原本外部無法存取
    std::shared_ptr<Soldier> m_TargetSoldier = nullptr;
    float m_AttackTimer = 0.0f;
    float m_AttackCooldown = 1.0f;

    std::shared_ptr<Util::Animation> m_MoveRightAni, m_MoveUpAni, m_MoveDownAni, m_AttackAni, m_DeadAni;
};

#endif