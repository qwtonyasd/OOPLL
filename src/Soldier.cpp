#include "Soldier.hpp"
#include "Util/Time.hpp"
#include "Util/Image.hpp"

const int WALK_START = 1, WALK_END = 6;
const int ATK_START = 7,  ATK_END = 17;
const int DEAD_START = 18, DEAD_END = 24;

Soldier::Soldier(glm::vec2 spawnPos, glm::vec2 rallyPoint, float speed, float hp)
    : Unit({spawnPos}, speed, hp), m_RallyPoint(rallyPoint) {
    m_Transform.translation = spawnPos;
    m_ZIndex = 15.0f;
    m_CurrentState = Soldier::State::MOVE_TO_RALLY;
}

void Soldier::Update(std::vector<std::shared_ptr<Enemy>>& enemies) {
    float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;
    if (m_HP <= 0) {
        if (m_CurrentState != Soldier::State::DEATH) SetState(Soldier::State::DEATH);
        UpdateAnimation(dt);
        return;
    }

    switch (m_CurrentState) {
        case Soldier::State::MOVE_TO_RALLY: MoveTowardsRallyPoint(); break;
        case Soldier::State::IDLE:          SearchForEnemy(enemies); break;
        case Soldier::State::BLOCKING:      PerformAttack(); break;
        case Soldier::State::DEATH:         break;
    }
    UpdateAnimation(dt);
}

void Soldier::MoveTowardsRallyPoint() {
    float dist = glm::distance(m_Transform.translation, m_RallyPoint);
    if (dist > 2.0f) {
        glm::vec2 dir = glm::normalize(m_RallyPoint - m_Transform.translation);
        m_Transform.translation += dir * m_Speed;
        m_Transform.scale.x = (dir.x > 0) ? 1.0f : -1.0f;
    } else {
        SetState(Soldier::State::IDLE);
    }
}

void Soldier::SearchForEnemy(std::vector<std::shared_ptr<Enemy>>& enemies) {
    for (auto& enemy : enemies) {
        if (enemy->GetHP() > 0 && !enemy->IsBlocked()) {
            if (glm::distance(m_Transform.translation, enemy->GetPosition()) < 40.0f) {
                m_TargetEnemy = enemy;
                SetState(Soldier::State::BLOCKING);
                return;
            }
        }
    }
}

void Soldier::PerformAttack() {
    if (!m_TargetEnemy || m_TargetEnemy->GetHP() <= 0) {
        ReleaseEnemy();
        SetState(Soldier::State::MOVE_TO_RALLY);
        return;
    }
    m_TargetEnemy->SetBlocked(true);
    m_Transform.scale.x = (m_TargetEnemy->GetPosition().x > m_Transform.translation.x) ? 1.0f : -1.0f;

    float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;
    m_TargetEnemy->TakeDamage(m_AttackDamage * dt);
}

void Soldier::SetState(Soldier::State newState) {
    m_CurrentState = newState;
    switch (newState) {
        case Soldier::State::MOVE_TO_RALLY: m_FrameIndex = WALK_START; break;
        case Soldier::State::IDLE:          m_FrameIndex = WALK_START; break;
        case Soldier::State::BLOCKING:      m_FrameIndex = ATK_START;  break;
        case Soldier::State::DEATH:         m_FrameIndex = DEAD_START; break;
    }
}

void Soldier::UpdateAnimation(float dt) {
    m_AnimTimer += dt;
    if (m_AnimTimer > 0.08f) {
        m_AnimTimer = 0;
        m_FrameIndex++;
        int start, end;
        bool loop = true;
        if (m_CurrentState == Soldier::State::BLOCKING) { start = ATK_START; end = ATK_END; }
        else if (m_CurrentState == Soldier::State::DEATH) { start = DEAD_START; end = DEAD_END; loop = false; }
        else { start = WALK_START; end = WALK_END; }

        if (m_FrameIndex > end) {
            if (loop) m_FrameIndex = start;
            else { m_FrameIndex = end; m_IsDeadAnimDone = true; }
        }
        SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/BarracksTower/Soldier/" + std::to_string(m_FrameIndex) + ".png"));
    }
}

void Soldier::ReleaseEnemy() {
    if (m_TargetEnemy) m_TargetEnemy->SetBlocked(false);
    m_TargetEnemy = nullptr;
}