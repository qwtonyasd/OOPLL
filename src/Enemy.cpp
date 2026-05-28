#include "Enemy.hpp"
#include "Soldier.hpp"

Enemy::Enemy(Enemy::Type type, const std::vector<glm::vec2>& path, float speed, float hp,
             const std::vector<std::vector<std::string>>& moveAnimations,
             const std::vector<std::string>& attackPaths,
             const std::vector<std::string>& deadPaths)
    : Unit(path, speed, hp), m_Type(type), m_CurrentState(State::MOVE_RIGHT) {

    m_MoveRightAni = std::make_shared<Util::Animation>(moveAnimations[0], true, 100, true, 0);
    m_MoveUpAni    = std::make_shared<Util::Animation>(moveAnimations[1], true, 100, true, 0);
    m_MoveDownAni  = std::make_shared<Util::Animation>(moveAnimations[2], true, 100, true, 0);
    m_AttackAni    = std::make_shared<Util::Animation>(attackPaths, true, 100, true, 0);
    m_DeadAni      = std::make_shared<Util::Animation>(deadPaths, false, 100, false, 0);

    SetDrawable(m_MoveRightAni);
}

void Enemy::Update(std::vector<std::shared_ptr<Enemy>>&, float dt) {
    if (m_HP <= 0 && m_CurrentState != State::DEATH) {
        OnDeath();
        return;
    }
    if (m_CurrentState == State::DEATH || m_CurrentState == State::SKILL) return;

    if (m_IsBlocked) {
        SetState(State::ATTACK);
        if (m_TargetSoldier && m_TargetSoldier->GetHP() > 0) {
            m_AttackTimer += dt;
            if (m_AttackTimer >= m_AttackCooldown) {
                m_TargetSoldier->TakeDamage(5.0f);
                m_AttackTimer = 0.0f;
            }
        } else {
            m_IsBlocked = false;
        }
    } else {
        MoveTowardsNextNode();
        UpdateDirection(m_Transform.translation - m_Transform.translation); // 簡化邏輯
    }
}

void Enemy::Draw() {
    if (m_HP <= 0 && IsDeadAnimationFinished()) return;
    GameObject::Draw();
    DrawHealthBar();
}

void Enemy::UpdateDirection(glm::vec2 dir) {
    if (std::abs(dir.x) > std::abs(dir.y)) {
        SetState(State::MOVE_RIGHT);
        SetDrawable(m_MoveRightAni);
    } else {
        SetState(dir.y > 0 ? State::MOVE_DOWN : State::MOVE_UP);
        SetDrawable(dir.y > 0 ? m_MoveDownAni : m_MoveUpAni);
    }
}

void Enemy::SetState(State newState) { m_CurrentState = newState; }

void Enemy::OnDeath() {
    SetState(State::DEATH);
    SetDrawable(m_DeadAni);
    m_DeadAni->SetCurrentFrame(0);
    m_DeadAni->Play();
}

bool Enemy::IsDeadAnimationFinished() const {
    return !m_DeadAni || m_DeadAni->GetState() == Util::Animation::State::ENDED;
}

void Enemy::TakeDamage(float damage, DamageType) {
    m_HP -= damage;
}