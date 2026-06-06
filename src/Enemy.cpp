#include "Enemy.hpp"
#include "Soldier.hpp"

// 建構子 (保持不變)
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
        if (m_CurrentState != State::ATTACK) {
            SetState(State::ATTACK);
            SetDrawable(m_AttackAni);
            m_AttackAni->Play();

            // 攻擊轉向
            if (m_TargetSoldier) {
                m_Transform.scale.x = (m_TargetSoldier->m_Transform.translation.x < m_Transform.translation.x) ? -1.0f : 1.0f;
            }
        }

        if (m_TargetSoldier && m_TargetSoldier->GetHP() > 0) {
            m_AttackTimer += dt;
            if (m_AttackTimer >= m_AttackCooldown) {
                m_TargetSoldier->TakeDamage(5.0f);
                m_AttackTimer = 0.0f;
            }
        } else {
            // --- 核心修正：小兵死亡，強制重置狀態為 MOVE_RIGHT 以解除 UpdateDirection 攔截 ---
            m_IsBlocked = false;
            m_TargetSoldier = nullptr;
            m_Transform.scale.x = 1.0f; // 重置翻轉

            // 這裡強制切回移動狀態
            SetState(State::MOVE_RIGHT);
            MoveTowardsNextNode(); // 先移動一小步
            UpdateDirection(m_MoveDirection); // 觸發動畫切換
        }
    } else {
        MoveTowardsNextNode();
        m_MoveDirection = (m_CurrentNodeIdx < m_Path.size()) ? (m_Path[m_CurrentNodeIdx] - m_Transform.translation) : m_MoveDirection;
        UpdateDirection(m_MoveDirection);
    }
}

void Enemy::UpdateDirection(glm::vec2 dir) {
    // 如果正在被 Block 且處於攻擊中，保持當前狀態不動
    if (m_IsBlocked && m_CurrentState == State::ATTACK) return;

    if (std::abs(dir.x) > std::abs(dir.y)) {
        // 水平移動
        SetState(State::MOVE_RIGHT);
        SetDrawable(m_MoveRightAni);

        // 核心邏輯：向右走(dir.x > 0)時，如果 scale 為 1.0f 還是反的，請將 1.0f 與 -1.0f 對調
        m_Transform.scale.x = (dir.x > 0) ? 1.0f : -1.0f;
    } else {
        // 垂直移動時，強制將 scale 重置為 1.0f，避免殘留水平翻轉
        m_Transform.scale.x = 1.0f;

        SetState(dir.y > 0 ? State::MOVE_DOWN : State::MOVE_UP);
        SetDrawable(dir.y > 0 ? m_MoveDownAni : m_MoveUpAni);
    }
}

void Enemy::MoveTowardsNextNode() {
    if (m_CurrentNodeIdx >= m_Path.size()) {
        m_ReachedEnd = true;
        return;
    }
    float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;
    glm::vec2 targetPos = m_Path[m_CurrentNodeIdx];
    glm::vec2 diff = targetPos - m_Transform.translation;

    if (glm::length(diff) > 1.0f) {
        m_Transform.translation += glm::normalize(diff) * m_Speed * dt;
    } else {
        m_CurrentNodeIdx++;
    }
}

void Enemy::Draw() {
    if (m_HP <= 0 && IsDeadAnimationFinished()) return;
    GameObject::Draw();
    DrawHealthBar();
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

// 🎯 核心修改：計算物理與魔法抗性的減傷實作
void Enemy::TakeDamage(float damage, DamageType damageType) {
    float finalDamage = damage;

    if (damageType == DamageType::PHYSICAL) {
        // 物理傷害依據護甲值（Armor Rating）進行減傷
        if (m_Armor == DefenseLevel::MEDIUM) {
            finalDamage *= 0.90f; // 減少 10% 傷害
        } else if (m_Armor == DefenseLevel::HIGH) {
            finalDamage *= 0.80f; // 減少 20% 傷害
        }
    }
    else if (damageType == DamageType::MAGIC) {
        // 魔法傷害依據魔抗（Magic Resistance）進行減傷
        if (m_MagicRes == DefenseLevel::MEDIUM) {
            finalDamage *= 0.90f; // 減少 10% 傷害
        } else if (m_MagicRes == DefenseLevel::HIGH) {
            finalDamage *= 0.80f; // 減少 20% 傷害
        }
    }

    // 扣除最終計算後的傷害值
    m_HP -= finalDamage;

    if (m_HP <= 0) {
        m_HP = 0;
        OnDeath();
    }
}