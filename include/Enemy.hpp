#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Unit.hpp"
#include "Util/Animation.hpp"
#include <vector>
#include <string>
#include <memory>
#include <cmath> // 用於 round

class Enemy : public Unit {
public:
    enum class Type { GOBLIN, ORC };
    enum class State { MOVE_RIGHT, MOVE_UP, MOVE_DOWN, ATTACK, DEATH };
    enum class DamageType { PHYSICAL, MAGIC }; // 新增傷害類型

    Enemy(Enemy::Type type, const std::vector<glm::vec2>& path, float speed, float hp,
          const std::vector<std::vector<std::string>>& moveAnimations,
          const std::vector<std::string>& attackPaths,
          const std::vector<std::string>& deadPaths);

    void Update() override;

    // 修改受傷函式，加入物理/魔法抗性邏輯
    void TakeDamage(float damage, DamageType damageType = DamageType::PHYSICAL);

    void UpdateDirection(glm::vec2 dir);
    void SetState(State newState);
    void OnDeath();

    bool IsDeadAnimationFinished() const;

    Enemy::Type GetType() const { return m_Type; }
    glm::vec2 GetPosition() const { return m_Transform.translation; }
    void SetBlocked(bool b) { m_IsBlocked = b; }
    bool IsBlocked() const { return m_IsBlocked; }
    float GetHP() const { return m_HP; }
    bool ReachedEnd() const { return m_ReachedEnd; }

private:
    Enemy::Type m_Type;
    State m_CurrentState;
    bool m_IsBlocked = false;
    bool m_ReachedEnd = false;
    std::shared_ptr<Util::Animation> m_MoveRightAni, m_MoveUpAni, m_MoveDownAni, m_AttackAni, m_DeadAni;
};

#endif