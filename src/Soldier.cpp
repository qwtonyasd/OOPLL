#include "Soldier.hpp"
#include "Util/Time.hpp"
#include "Util/Image.hpp"
#include <random>

// 動畫幀數設定
const int WALK_START = 1, WALK_END = 6;
const int ATK_START = 7,  ATK_END = 17;
const int DEAD_START = 18, DEAD_END = 24;

// 隨機數產生器
static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_real_distribution<float> dis(2.0f, 5.0f);
static std::uniform_int_distribution<int> damageDis(5, 10); // 假設傷害在 5~10 之間

Soldier::Soldier(glm::vec2 spawnPos, glm::vec2 rallyPoint, float speed, float hp)
    : Unit({spawnPos}, speed, hp), m_RallyPoint(rallyPoint) {
    m_HP = hp;
    m_Transform.translation = spawnPos;
    m_ZIndex = 15.0f;

    // 初始化計時器
    m_AnimTimer = 0.0f;
    m_AttackTimer = 0.0f;
    m_TurnTimer = 0.0f;
    m_NextTurnTime = dis(gen);

    // 設定初始圖片，避免第一幀空白
    SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/BarracksTower/Soldier/1.png"));

    SetState(Soldier::State::MOVE_TO_RALLY);
}

void Soldier::Update(std::vector<std::shared_ptr<Enemy>>& enemies) {
    float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;
    if (dt > 0.1f) dt = 0.016f; // 防止跳幀導致邏輯崩潰

    if (m_HP <= 0) {
        if (m_CurrentState != Soldier::State::DEATH) {
            SetState(Soldier::State::DEATH);
            ReleaseEnemy();
        }
        UpdateAnimation(dt);
        return;
    }

    switch (m_CurrentState) {
        case Soldier::State::MOVE_TO_RALLY:
            MoveTowardsRallyPoint();
            break;
        case Soldier::State::IDLE:
            SearchForEnemy(enemies);
            UpdateIdleRotation(dt);
            break;
        case Soldier::State::BLOCKING:
            PerformAttack(dt);
            break;
        case Soldier::State::DEATH:
            break;
    }

    UpdateAnimation(dt);
}

void Soldier::MoveTowardsRallyPoint() {
    float dist = glm::distance(m_Transform.translation, m_RallyPoint);
    float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;

    if (dist > 3.0f) {
        if (m_CurrentState != State::MOVE_TO_RALLY) SetState(State::MOVE_TO_RALLY);

        glm::vec2 dir = glm::normalize(m_RallyPoint - m_Transform.translation);
        m_Transform.translation += dir * m_Speed * (dt * 60.0f); // 確保速度與幀率無關
        m_Transform.scale.x = (dir.x > 0) ? 1.0f : -1.0f;
    } else {
        m_Transform.translation = m_RallyPoint; // 精確修正位置
        SetState(Soldier::State::IDLE);
    }
}

void Soldier::SearchForEnemy(std::vector<std::shared_ptr<Enemy>>& enemies) {
    for (auto& enemy : enemies) {
        if (enemy && enemy->GetHP() > 0 && !enemy->IsBlocked()) {
            // 偵測距離 45.0f (可依需求調整)
            if (glm::distance(m_Transform.translation, enemy->GetPosition()) < 45.0f) {
                m_TargetEnemy = enemy;
                // 關鍵：使用 shared_from_this() 傳遞自己
                m_TargetEnemy->SetBlocked(true, shared_from_this());
                SetState(Soldier::State::BLOCKING);
                return;
            }
        }
    }
}

void Soldier::PerformAttack(float dt) {
    if (!m_TargetEnemy || m_TargetEnemy->GetHP() <= 0) {
        ReleaseEnemy();
        SetState(Soldier::State::MOVE_TO_RALLY);
        return;
    }

    // 面向敵人
    m_Transform.scale.x = (m_TargetEnemy->GetPosition().x > m_Transform.translation.x) ? 1.0f : -1.0f;

    // 攻擊冷卻計時
    m_AttackTimer += dt;
    if (m_AttackTimer >= m_AttackCooldown) { // 預設 1.0 秒攻擊一次
        m_TargetEnemy->TakeDamage(static_cast<float>(damageDis(gen)));
        m_AttackTimer = 0.0f;
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
    if (m_HP > 0) {
        m_HP -= damage;
    }
}

void Soldier::SetState(Soldier::State newState) {
    if (m_CurrentState == newState && newState != State::IDLE) return;

    m_CurrentState = newState;
    m_AnimTimer = 0.0f;
    // 進入新狀態時重置攻擊計時，確保第一擊不會秒發
    m_AttackTimer = 0.0f;

    switch (newState) {
        case Soldier::State::MOVE_TO_RALLY: m_FrameIndex = WALK_START; break;
        case Soldier::State::IDLE:          m_FrameIndex = WALK_START; m_TurnTimer = 0.0f; break;
        case Soldier::State::BLOCKING:      m_FrameIndex = ATK_START;  break;
        case Soldier::State::DEATH:         m_FrameIndex = DEAD_START; break;
    }
}

void Soldier::UpdateAnimation(float dt) {
    m_AnimTimer += dt;
    if (m_AnimTimer > 0.08f) { // 約 12 FPS
        m_AnimTimer = 0;
        int start = 1, end = 1;
        bool loop = true;

        switch (m_CurrentState) {
            case State::MOVE_TO_RALLY: start = WALK_START; end = WALK_END; break;
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

void Soldier::ReleaseEnemy() {
    if (m_TargetEnemy) {
        m_TargetEnemy->SetBlocked(false, nullptr);
    }
    m_TargetEnemy = nullptr;
}