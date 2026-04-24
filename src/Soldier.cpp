#include "Soldier.hpp"
#include "Enemy.hpp"
#include "Util/Time.hpp"
#include "Util/Image.hpp"
#include <random>

const int WALK_START = 1, WALK_END = 6;
const int ATK_START = 7,  ATK_END = 17;
const int DEAD_START = 18, DEAD_END = 24;

static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_real_distribution<float> dis(2.0f, 5.0f);
static std::uniform_int_distribution<int> damageDis(8, 12);

Soldier::Soldier(glm::vec2 spawnPos, glm::vec2 rallyPoint, float speed, float hp)
    : Unit({spawnPos}, speed, hp), m_RallyPoint(rallyPoint) {
    m_HP = hp;
    m_MaxHP = hp;
    m_Transform.translation = spawnPos;
    m_ZIndex = 15.0f;
    m_AnimTimer = 0.0f;
    m_AttackTimer = 0.0f;
    m_TurnTimer = 0.0f;
    m_NextTurnTime = dis(gen);
    m_DetectionRange = 80.0f;

    SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/BarracksTower/Soldier/1.png"));
    SetState(Soldier::State::MOVE_TO_RALLY);
}

void Soldier::Update(std::vector<std::shared_ptr<Enemy>>& enemies) {
    float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;
    if (dt > 0.1f) dt = 0.016f;

    if (m_HP <= 0) {
        if (m_CurrentState != State::DEATH) {
            ReleaseEnemy();
            SetState(State::DEATH);
        }
        UpdateAnimation(dt);
        return;
    }

    // 只有在閒置或移動回集結點時，才去尋找新目標
    if (m_CurrentState == State::IDLE || m_CurrentState == State::MOVE_TO_RALLY) {
        SearchForEnemy(enemies);
    }

    switch (m_CurrentState) {
        case State::MOVE_TO_RALLY: MoveTowardsRallyPoint(); break;
        case State::IDLE:          UpdateIdleRotation(dt); break;
        case State::CHASE:         ChaseEnemy(dt); break;
        case State::BLOCKING:      PerformAttack(dt); break;
        case State::DEATH:         break;
    }
    UpdateAnimation(dt);
}

void Soldier::Draw() {
    if (m_HP <= 0 && m_IsDeadAnimDone) return;
    GameObject::Draw();
    DrawHealthBar();
}

void Soldier::EngageTarget(std::shared_ptr<Enemy> enemy) {
    if (IsAvailable() && enemy) {
        m_TargetEnemy = enemy;
        SetState(State::CHASE);
    }
}

bool Soldier::IsAvailable() const {
    return (m_CurrentState == State::IDLE || m_CurrentState == State::MOVE_TO_RALLY);
}

void Soldier::ChaseEnemy(float dt) {
    if (!m_TargetEnemy || m_TargetEnemy->GetHP() <= 0 || m_TargetEnemy->ReachedEnd()) {
        m_TargetEnemy = nullptr;
        SetState(State::MOVE_TO_RALLY);
        return;
    }

    float dist = glm::distance(m_Transform.translation, m_TargetEnemy->GetPosition());

    // 關鍵修正：只有在走到了近戰範圍內，才讓怪物停下 (SetBlocked)
    if (dist > m_MeleeRange) {
        glm::vec2 dir = glm::normalize(m_TargetEnemy->GetPosition() - m_Transform.translation);
        m_Transform.translation += dir * m_Speed * (dt * 60.0f);
        m_Transform.scale.x = (dir.x > 0) ? 1.0f : -1.0f;
    } else {
        // 走到了！現在才正式「攔截」怪物
        if (!m_TargetEnemy->IsBlocked()) {
            m_TargetEnemy->SetBlocked(true, shared_from_this());
        }
        SetState(State::BLOCKING);
    }
}

void Soldier::PerformAttack(float dt) {
    if (!m_TargetEnemy || m_TargetEnemy->GetHP() <= 0 || m_TargetEnemy->ReachedEnd()) {
        m_TargetEnemy = nullptr;
        SetState(State::MOVE_TO_RALLY);
        return;
    }

    // 如果攔截者（主要擋路的人）死了，怪物會變回 !IsBlocked
    // 這時候還活著的支援者需要立刻「補位」變成攔截者，否則怪物會走掉
    if (!m_TargetEnemy->IsBlocked()) {
        m_TargetEnemy->SetBlocked(true, shared_from_this());
    }

    m_Transform.scale.x = (m_TargetEnemy->GetPosition().x > m_Transform.translation.x) ? 1.0f : -1.0f;

    m_AttackTimer += dt;
    if (m_AttackTimer >= m_AttackCooldown) {
        m_TargetEnemy->TakeDamage(static_cast<float>(damageDis(gen)));
        m_AttackTimer = 0.0f;
    }
}

void Soldier::MoveTowardsRallyPoint() {
    float dist = glm::distance(m_Transform.translation, m_RallyPoint);
    float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;

    if (dist > 3.0f) {
        glm::vec2 dir = glm::normalize(m_RallyPoint - m_Transform.translation);
        m_Transform.translation += dir * m_Speed * (dt * 60.0f);
        m_Transform.scale.x = (dir.x > 0) ? 1.0f : -1.0f;
    } else {
        m_Transform.translation = m_RallyPoint;
        SetState(State::IDLE);
    }
}

void Soldier::SetState(State newState) {
    if (m_CurrentState == newState && newState != State::IDLE) return;
    m_CurrentState = newState;
    m_AnimTimer = 0.0f;
    m_AttackTimer = 0.0f;

    switch (newState) {
        case State::MOVE_TO_RALLY:
        case State::CHASE:         m_FrameIndex = WALK_START; break;
        case State::IDLE:          m_FrameIndex = WALK_START; break;
        case State::BLOCKING:      m_FrameIndex = ATK_START;  break;
        case State::DEATH:         m_FrameIndex = DEAD_START; break;
    }
}

void Soldier::UpdateAnimation(float dt) {
    m_AnimTimer += dt;
    if (m_AnimTimer > 0.08f) {
        m_AnimTimer = 0;
        int start = 1, end = 1;
        bool loop = true;

        switch (m_CurrentState) {
            case State::MOVE_TO_RALLY:
            case State::CHASE:         start = WALK_START; end = WALK_END; break;
            case State::BLOCKING:      start = ATK_START;  end = ATK_END;  break;
            case State::DEATH:         start = DEAD_START; end = DEAD_END; loop = false; break;
            case State::IDLE:          start = WALK_START; end = WALK_START; break;
        }

        m_FrameIndex++;
        if (m_FrameIndex > end) {
            if (loop) m_FrameIndex = start;
            else {
                m_FrameIndex = end;
                m_IsDeadAnimDone = true;
            }
        } else if (m_FrameIndex < start) {
            m_FrameIndex = start;
        }

        SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/BarracksTower/Soldier/" + std::to_string(m_FrameIndex) + ".png"));
    }
}

void Soldier::UpdateIdleRotation(float dt) {
    m_TurnTimer += dt;
    if (m_TurnTimer >= m_NextTurnTime) {
        m_TurnTimer = 0.0f;
        m_Transform.scale.x *= -1.0f;
        m_NextTurnTime = dis(gen);
    }
}

void Soldier::TakeDamage(float damage) {
    if (m_HP > 0) m_HP -= damage;
}

void Soldier::ReleaseEnemy() {
    if (m_TargetEnemy) {
        m_TargetEnemy->SetBlocked(false, nullptr);
    }
    m_TargetEnemy = nullptr;
}

void Soldier::SearchForEnemy(std::vector<std::shared_ptr<Enemy>>& enemies) {
    std::shared_ptr<Enemy> bestTarget = nullptr;
    float minDistance = m_DetectionRange;

    // 階段 1：找沒人擋的怪
    for (auto& enemy : enemies) {
        if (!enemy || enemy->GetHP() <= 0 || enemy->ReachedEnd()) continue;
        if (!enemy->IsBlocked()) {
            float dist = glm::distance(m_Transform.translation, enemy->GetPosition());
            if (dist < minDistance) {
                minDistance = dist;
                bestTarget = enemy;
            }
        }
    }

    // 階段 2：沒自由怪才找圍毆目標
    if (!bestTarget) {
        minDistance = m_DetectionRange;
        for (auto& enemy : enemies) {
            if (!enemy || enemy->GetHP() <= 0 || enemy->ReachedEnd()) continue;
            float dist = glm::distance(m_Transform.translation, enemy->GetPosition());
            if (dist < minDistance) {
                minDistance = dist;
                bestTarget = enemy;
            }
        }
    }

    if (bestTarget) {
        EngageTarget(bestTarget); // 這裡不再調用 SetBlocked，讓 ChaseEnemy 走到目的地後再攔截
    }
}