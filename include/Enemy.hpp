#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Unit.hpp"
#include "Util/Animation.hpp"
#include <vector>
#include <string>
#include <memory>
#include <glm/glm.hpp>

class Soldier;

class Enemy : public Unit {
public:
    enum class Type { GOBLIN, ORC, WULF, SHAMAN, OGRE, WORG, BANDIT, BRIGAND, MARAUDER, GIANT_SPIDER, SPIDER_MATRIARCH, EGG };
    enum class State { MOVE_RIGHT, MOVE_UP, MOVE_DOWN, ATTACK, SKILL, DEATH };
    enum class DamageType { PHYSICAL, MAGIC };

    Enemy(Enemy::Type type, const std::vector<glm::vec2>& path, float speed, float hp,
          const std::vector<std::vector<std::string>>& moveAnimations,
          const std::vector<std::string>& attackPaths,
          const std::vector<std::string>& deadPaths);

    virtual ~Enemy() = default;

    // 已修正：與 Unit.hpp 的簽章完全一致
    virtual void Update(std::vector<std::shared_ptr<Enemy>>& enemies, float dt) override;
    virtual void Draw() override;
    void MoveTowardsNextNode();
    void TakeDamage(float damage, DamageType damageType = DamageType::PHYSICAL);
    void UpdateDirection(glm::vec2 dir);
    void SetState(State newState);
    void OnDeath();
    bool IsDeadAnimationFinished() const;

    void Heal(float amount) {
        if (m_CurrentState == State::DEATH) return;
        m_HP += amount;
        if (m_HP > m_MaxHP) m_HP = m_MaxHP;
    }

    bool ReachedEnd() const { return m_ReachedEnd; }
    float GetTotalTravelledDistance() const { return m_TotalDistanceTravelled; }
    Enemy::Type GetType() const { return m_Type; }
    bool IsBlocked() const { return m_IsBlocked; }
    glm::vec2 GetPosition() const { return m_Transform.translation; }

    void SetBlocked(bool b, std::shared_ptr<Soldier> soldier = nullptr) {
        m_IsBlocked = b;
        m_TargetSoldier = soldier;
    }

protected:
    glm::vec2 m_MoveDirection = {1.0f, 0.0f}; // 初始預設朝右
    Enemy::Type m_Type;
    State m_CurrentState;

    bool m_IsBlocked = false;
    bool m_ReachedEnd = false;
    float m_TotalDistanceTravelled = 0.0f;

    std::shared_ptr<Soldier> m_TargetSoldier = nullptr;
    float m_AttackTimer = 0.0f;
    float m_AttackCooldown = 1.0f;

    std::shared_ptr<Util::Animation> m_MoveRightAni, m_MoveUpAni, m_MoveDownAni, m_AttackAni, m_DeadAni;
};

#endif