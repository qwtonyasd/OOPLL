#include "Enemy.hpp"
#include "Soldier.hpp"
#include "Util/Time.hpp"
#include <cmath> // 確保 std::round 可用

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
    float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;

    if (m_HP <= 0 && m_CurrentState != State::DEATH) {
        OnDeath();
        return;
    }
    if (m_CurrentState == State::DEATH) return;

    // 額外保險：如果當前狀態是 SKILL，不執行以下移動與普攻計時邏輯（由 Shaman 內部自行阻斷與轉換）
    if (m_CurrentState == State::SKILL) return;

    if (m_IsBlocked) {
        SetState(State::ATTACK);

        if (m_TargetSoldier && m_TargetSoldier->GetHP() > 0) {
            m_AttackTimer += dt;
            if (m_AttackTimer >= m_AttackCooldown) {
                // 根據怪物類型分配傷害 (新加入強大的 Ogre 傷害)
                float dmg = 5.0f;
                if (m_Type == Type::ORC)       dmg = 12.0f;
                else if (m_Type == Type::OGRE) dmg = 25.0f;  // Ogre 是重擊型 Boss 傷害給 25
                else if (m_Type == Type::WULF) dmg = 8.0f;   // 補上狼與薩滿的基本普攻參考傷害
                else if (m_Type == Type::SHAMAN) dmg = 6.0f;

                m_TargetSoldier->TakeDamage(dmg);
                m_AttackTimer = 0.0f;
            }
        } else {
            SetBlocked(false, nullptr);
        }
    }

    if (!m_IsBlocked) {
        if (m_CurrentNodeIdx < m_Path.size()) {
            glm::vec2 oldPos = m_Transform.translation;
            MoveTowardsNextNode();

            m_TotalDistanceTravelled += glm::distance(m_Transform.translation, oldPos);

            glm::vec2 dir = m_Transform.translation - oldPos;
            if (glm::length(dir) > 0.1f) UpdateDirection(dir);
        } else {
            m_ReachedEnd = true;
            m_HP = 0;
        }
    }
}

// --- Draw 函式 ---
void Enemy::Draw() {
    if (m_HP <= 0 && IsDeadAnimationFinished()) return;

    // 1. 呼叫基底類別的 Draw 來繪製怪物本體動畫
    GameObject::Draw();
    DrawHealthBar();
}

void Enemy::TakeDamage(float damage, DamageType damageType) {
    float finalDamage = damage;

    // Orc 擁有物理減傷
    if (m_Type == Enemy::Type::ORC && damageType == DamageType::PHYSICAL) {
        finalDamage = std::round(damage / 1.5f);
    }
    // 新增：Ogre 身為大 Boss，可以給予不分魔物傷的全面 20% 減傷 (除以 1.25)
    else if (m_Type == Enemy::Type::OGRE) {
        finalDamage = std::round(damage / 1.25f);
    }

    if (finalDamage < 1.0f) finalDamage = 1.0f;

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
    m_AttackTimer = 0.0f;
    switch (m_CurrentState) {
        case State::MOVE_RIGHT: SetDrawable(m_MoveRightAni); break;
        case State::MOVE_UP:    SetDrawable(m_MoveUpAni);    break;
        case State::MOVE_DOWN:  SetDrawable(m_MoveDownAni);  break;
        case State::ATTACK:     SetDrawable(m_AttackAni);    break;
        case State::DEATH:      SetDrawable(m_DeadAni);      break;
        case State::SKILL:                                   break; // 修正：補上 case 消除 -Wswitch 警告
    }
}

void Enemy::OnDeath() {
    SetState(State::DEATH);
    m_IsBlocked = false;
    m_TargetSoldier = nullptr;
    m_DeadAni->SetLooping(false);
    m_DeadAni->SetCurrentFrame(0);
    m_DeadAni->Play();
}

bool Enemy::IsDeadAnimationFinished() const {
    if (m_CurrentState != State::DEATH) return false;
    if (!m_DeadAni) return true;
    return (m_DeadAni->GetState() == Util::Animation::State::ENDED) ||
           (m_DeadAni->GetCurrentFrameIndex() >= m_DeadAni->GetFrameCount() - 1);
}