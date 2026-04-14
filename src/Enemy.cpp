#include "Enemy.hpp"

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
    SetZIndex(10.0f);
}

void Enemy::Update() {
    if (m_HP <= 0 && m_CurrentState != State::DEATH) {
        OnDeath();
        return;
    }
    if (m_CurrentState == State::DEATH) return;

    if (!m_IsBlocked) {
        if (m_CurrentNodeIdx < m_Path.size()) {
            glm::vec2 oldPos = m_Transform.translation;
            MoveTowardsNextNode();
            glm::vec2 dir = m_Transform.translation - oldPos;
            if (glm::length(dir) > 0.1f) UpdateDirection(dir);
        } else {
            m_ReachedEnd = true;
            m_HP = 0;
        }
    } else {
        SetState(State::ATTACK);
    }
}

void Enemy::TakeDamage(float damage, DamageType damageType) {
    float finalDamage = damage;

    // 獸人護甲邏輯：受到物理傷害時，傷害 / 1.5
    if (m_Type == Enemy::Type::ORC && damageType == DamageType::PHYSICAL) {
        // 使用 std::round 進行四捨五入
        finalDamage = std::round(damage / 1.5f);
        
        // 最低傷害限制為 1
        if (finalDamage < 1.0f) {
            finalDamage = 1.0f;
        }
    }

    m_HP -= finalDamage;
    if (m_HP < 0) m_HP = 0;
}

void Enemy::UpdateDirection(glm::vec2 dir) {
    if (std::abs(dir.x) > std::abs(dir.y)) {
        SetState(State::MOVE_RIGHT);
        m_Transform.scale.x = (dir.x > 0) ? 1.0f : -1.0f;
    } else {
        m_Transform.scale.x = 1.0f;
        if (dir.y > 0) SetState(State::MOVE_UP);
        else SetState(State::MOVE_DOWN);
    }
}

void Enemy::SetState(State newState) {
    if (m_CurrentState == newState) return;
    m_CurrentState = newState;
    switch (m_CurrentState) {
        case State::MOVE_RIGHT: SetDrawable(m_MoveRightAni); break;
        case State::MOVE_UP:    SetDrawable(m_MoveUpAni);    break;
        case State::MOVE_DOWN:  SetDrawable(m_MoveDownAni);  break;
        case State::ATTACK:     SetDrawable(m_AttackAni);    break;
        case State::DEATH:      SetDrawable(m_DeadAni);      break;
    }
}

void Enemy::OnDeath() {
    SetState(State::DEATH);
    m_IsBlocked = false;
    m_DeadAni->SetLooping(false);
    m_DeadAni->SetCurrentFrame(0);
    m_DeadAni->Play();
}

bool Enemy::IsDeadAnimationFinished() const {
    if (m_CurrentState != State::DEATH) return false;
    return (m_DeadAni->GetState() == Util::Animation::State::ENDED) ||
           (m_DeadAni->GetCurrentFrameIndex() >= m_DeadAni->GetFrameCount() - 1);
}