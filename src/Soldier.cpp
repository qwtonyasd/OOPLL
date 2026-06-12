#include "Soldier.hpp"
#include "Enemy.hpp"
#include "Util/Image.hpp"
#include <random>
#include <cmath>

static std::random_device rd;
static std::mt19937 gen(rd());

Soldier::Soldier(glm::vec2 spawnPos, glm::vec2 rallyPoint, const SoldierConfig& config)
    : Unit({spawnPos}, config.speed, config.maxHP), m_RallyPoint(rallyPoint), m_Config(config) {

    m_HP = m_Config.maxHP;
    m_MaxHP = m_Config.maxHP;
    m_Transform.translation = spawnPos;
    m_ZIndex = 15.0f;

    m_FrameIndex = m_Config.walkStart;
    m_FacingRight = (m_RallyPoint.x >= spawnPos.x); // 初始化面向集結點

    SetDrawable(std::make_shared<Util::Image>(m_Config.spriteRootPath + std::to_string(m_FrameIndex) + ".png"));
    SetState(State::MOVE_TO_RALLY);
}

void Soldier::Update(std::vector<std::shared_ptr<Enemy>>& enemies, float dt) {
    if (m_HP <= 0) {
        if (m_CurrentState != State::DEATH) {
            ReleaseEnemy();
            SetState(State::DEATH);
        }
        UpdateAnimation(dt);
        return;
    }

    if (m_CurrentState == State::IDLE || m_CurrentState == State::MOVE_TO_RALLY) {
        SearchForEnemy(enemies);
    }
    else if (m_CurrentState == State::CHASE || m_CurrentState == State::BLOCKING) {
        static float reScanTimer = 0.0f;
        reScanTimer += dt;
        if (reScanTimer >= 0.15f) {
            reScanTimer = 0.0f;
            SearchForEnemy(enemies);
        }
    }

    if (m_TargetEnemy) {
        float distToRally = glm::distance(m_RallyPoint, m_TargetEnemy->GetPosition());
        if (distToRally > 120.0f) {
            ReleaseEnemy();
            SetState(State::MOVE_TO_RALLY);
        }
    }

    switch (m_CurrentState) {
        case State::MOVE_TO_RALLY: MoveTowardsRallyPoint(dt); break;
        case State::IDLE:          UpdateIdleRotation(dt); break;
        case State::CHASE:         ChaseEnemy(dt); break;
        case State::BLOCKING:      PerformAttack(dt); break;
        case State::DEATH:         break;
    }
    UpdateAnimation(dt); // 🎯 轉向將在此函式內被強制套用
}

void Soldier::Draw() {
    if (m_HP <= 0 && m_IsDeadAnimDone) return;

    Util::Transform originalTransform = m_Transform;
    m_Transform.translation.x = std::round(m_Transform.translation.x);
    m_Transform.translation.y = std::round(m_Transform.translation.y);
    m_Transform.scale = glm::vec2(m_FacingRight ? 1.0f : -1.0f, 1.0f); // 確保繪製時套用正確面向

    if (m_Drawable) {
        glm::vec2 imgSize = m_Drawable->GetSize();
        if (static_cast<int>(imgSize.x) % 2 != 0) m_Transform.translation.x += 0.5f;
        if (static_cast<int>(imgSize.y) % 2 != 0) m_Transform.translation.y += 0.5f;
    }

    GameObject::Draw();
    DrawHealthBar();

    m_Transform = originalTransform;
}

void Soldier::EngageTarget(std::shared_ptr<Enemy> enemy) {
    if (enemy) {
        m_TargetEnemy = enemy;
        m_IsMainBlocker = false;
        SetState(State::CHASE);
    }
}

bool Soldier::IsAvailable() const {
    return (m_CurrentState == State::IDLE ||
            m_CurrentState == State::MOVE_TO_RALLY ||
            (m_CurrentState == State::BLOCKING && !m_IsMainBlocker));
}

void Soldier::ChaseEnemy(float dt) {
    if (!m_TargetEnemy || m_TargetEnemy->GetHP() <= 0 || m_TargetEnemy->ReachedEnd()) {
        ReleaseEnemy();
        SetState(State::MOVE_TO_RALLY);
        return;
    }

    float dist = glm::distance(m_Transform.translation, m_TargetEnemy->GetPosition());

    if (m_TargetEnemy->IsBlocked() && dist > m_Config.detectionRange) {
        ReleaseEnemy();
        SetState(State::MOVE_TO_RALLY);
        return;
    }

    if (dist > m_Config.meleeRange) {
        glm::vec2 dir = glm::normalize(m_TargetEnemy->GetPosition() - m_Transform.translation);
        m_Transform.translation += dir * m_Config.speed * (dt * 60.0f);

        // 🎯 紀錄面向方向
        m_FacingRight = (dir.x > 0);
    } else {
        if (!m_TargetEnemy->IsBlocked()) {
            m_TargetEnemy->SetBlocked(true, shared_from_this());
            m_IsMainBlocker = true;
        } else {
            m_IsMainBlocker = false;
        }
        SetState(State::BLOCKING);
    }
}

void Soldier::PerformAttack(float dt) {
    if (!m_TargetEnemy || m_TargetEnemy->GetHP() <= 0 || m_TargetEnemy->ReachedEnd()) {
        ReleaseEnemy();
        SetState(State::MOVE_TO_RALLY);
        return;
    }

    if (!m_TargetEnemy->IsBlocked()) {
        m_TargetEnemy->SetBlocked(true, shared_from_this());
        m_IsMainBlocker = true;
    }

    // 🎯 攻擊時精準面向敵人 X 軸位置
    m_FacingRight = (m_TargetEnemy->GetPosition().x > m_Transform.translation.x);

    m_AttackTimer += dt;
    if (m_AttackTimer >= m_Config.attackCooldown) {
        std::uniform_int_distribution<int> damageDis(m_Config.minDamage, m_Config.maxDamage);
        m_TargetEnemy->TakeDamage(static_cast<float>(damageDis(gen)));
        m_AttackTimer = 0.0f;
    }
}

void Soldier::MoveTowardsRallyPoint(float dt) {
    float dist = glm::distance(m_Transform.translation, m_RallyPoint);

    if (dist > 3.0f) {
        glm::vec2 dir = glm::normalize(m_RallyPoint - m_Transform.translation);
        m_Transform.translation += dir * m_Config.speed * (dt * 60.0f);

        // 🎯 紀錄面向方向
        m_FacingRight = (dir.x > 0);
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
        case State::CHASE:         m_FrameIndex = m_Config.walkStart; break;
        case State::IDLE:          m_FrameIndex = m_Config.walkStart; break;
        case State::BLOCKING:      m_FrameIndex = m_Config.atkStart;  break;
        case State::DEATH:         m_FrameIndex = m_Config.deadStart; break;
    }

    SetDrawable(std::make_shared<Util::Image>(m_Config.spriteRootPath + std::to_string(m_FrameIndex) + ".png"));
}

void Soldier::UpdateAnimation(float dt) {
    m_AnimTimer += dt;
    if (m_AnimTimer > 0.08f) {
        m_AnimTimer = 0.0f;
        int start = m_Config.walkStart, end = m_Config.walkStart;
        bool loop = true;

        switch (m_CurrentState) {
            case State::MOVE_TO_RALLY:
            case State::CHASE:         start = m_Config.walkStart; end = m_Config.walkEnd; break;
            case State::BLOCKING:      start = m_Config.atkStart;  end = m_Config.atkEnd;  break;
            case State::DEATH:         start = m_Config.deadStart; end = m_Config.deadEnd; loop = false; break;
            case State::IDLE:          start = m_Config.walkStart; end = m_Config.walkStart; break;
        }

        m_FrameIndex++;
        if (m_FrameIndex > end) {
            m_FrameIndex = loop ? start : end;
            if (!loop) m_IsDeadAnimDone = true;
        }

        if (m_FrameIndex < start || m_FrameIndex > end) {
            m_FrameIndex = start;
        }

        SetDrawable(std::make_shared<Util::Image>(m_Config.spriteRootPath + std::to_string(m_FrameIndex) + ".png"));
    }

    // 🎯 【關鍵修復】不管 SetDrawable 怎麼覆蓋，都在最後一刻將正確的面向套用回 Transform
    m_Transform.scale.x = m_FacingRight ? 1.0f : -1.0f;
}

void Soldier::UpdateIdleRotation(float dt) {
    m_TurnTimer += dt;
    if (m_TurnTimer >= m_NextTurnTime) {
        m_TurnTimer = 0.0f;

        // 🎯 改為翻轉狀態變數
        m_FacingRight = !m_FacingRight;

        std::uniform_real_distribution<float> dis(2.0f, 5.0f);
        m_NextTurnTime = dis(gen);
    }
}

void Soldier::TakeDamage(float damage) {
    if (m_HP > 0) m_HP -= damage;
}

void Soldier::ReleaseEnemy() {
    if (m_TargetEnemy) {
        if (m_IsMainBlocker) {
            m_TargetEnemy->SetBlocked(false, shared_from_this());
        }
    }
    m_TargetEnemy = nullptr;
    m_IsMainBlocker = false;
    m_AttackTimer = 0.0f;
}

void Soldier::SearchForEnemy(std::vector<std::shared_ptr<Enemy>>& enemies) {
    std::shared_ptr<Enemy> bestFreeEnemy = nullptr;
    std::shared_ptr<Enemy> bestBlockedEnemy = nullptr;

    float extendedRange = m_Config.detectionRange + 30.0f;
    float minFreeDist = extendedRange;
    float minBlockedDist = extendedRange;

    for (auto& enemy : enemies) {
        if (!enemy || enemy->GetHP() <= 0 || enemy->ReachedEnd()) continue;

        float distToMe = glm::distance(m_Transform.translation, enemy->GetPosition());
        float distToRally = glm::distance(m_RallyPoint, enemy->GetPosition());

        if (distToMe < extendedRange || distToRally < extendedRange) {
            if (!enemy->IsBlocked()) {
                if (distToMe < minFreeDist) {
                    minFreeDist = distToMe;
                    bestFreeEnemy = enemy;
                }
            } else {
                if (distToMe < minBlockedDist) {
                    minBlockedDist = distToMe;
                    bestBlockedEnemy = enemy;
                }
            }
        }
    }

    if (bestFreeEnemy) {
        if (m_TargetEnemy && m_TargetEnemy != bestFreeEnemy && !m_IsMainBlocker) {
            ReleaseEnemy();
            EngageTarget(bestFreeEnemy);
        }
        else if (!m_TargetEnemy) {
            EngageTarget(bestFreeEnemy);
        }
        return;
    }

    if (bestBlockedEnemy) {
        if (!m_TargetEnemy) {
            EngageTarget(bestBlockedEnemy);
        }
        return;
    }
}

void Soldier::Upgrade(const SoldierConfig& newConfig) {
    m_Config = newConfig;

    float hpOffset = newConfig.maxHP - m_MaxHP;
    m_MaxHP = newConfig.maxHP;
    if (hpOffset > 0 && m_HP > 0) {
        m_HP += hpOffset;
    }

    switch (m_CurrentState) {
        case State::MOVE_TO_RALLY:
        case State::CHASE:         m_FrameIndex = m_Config.walkStart; break;
        case State::IDLE:          m_FrameIndex = m_Config.walkStart; break;
        case State::BLOCKING:      m_FrameIndex = m_Config.atkStart;  break;
        case State::DEATH:         m_FrameIndex = m_Config.deadStart; break;
    }

    if (m_HP > 0) {
        SetDrawable(std::make_shared<Util::Image>(m_Config.spriteRootPath + std::to_string(m_FrameIndex) + ".png"));
    }

    // 升級時也確保立刻重載面向
    m_Transform.scale.x = m_FacingRight ? 1.0f : -1.0f;
}