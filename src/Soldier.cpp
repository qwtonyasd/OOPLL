#include "Soldier.hpp"
#include "Enemy.hpp"
#include "Util/Image.hpp"
#include <random>
#include <cmath>
#include <algorithm>

static std::random_device rd;
static std::mt19937 gen(rd());

Soldier::Soldier(glm::vec2 spawnPos, glm::vec2 rallyPoint, const SoldierConfig& config)
    : Unit({spawnPos}, config.speed, config.maxHP), m_RallyPoint(rallyPoint), m_Config(config) {

    m_HP = m_Config.maxHP;
    m_MaxHP = m_Config.maxHP;
    m_Transform.translation = spawnPos;
    m_ZIndex = 15.0f;

    m_FrameIndex = m_Config.walkStart;
    m_FacingRight = (m_RallyPoint.x >= spawnPos.x);

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

    // 每 0.02 秒獨立高靈敏度掃描一次戰場
    if (m_CurrentState == State::IDLE || m_CurrentState == State::MOVE_TO_RALLY) {
        SearchForEnemy(enemies);
    }
    else if (m_CurrentState == State::CHASE || m_CurrentState == State::BLOCKING) {
        m_ReScanTimer += dt;
        if (m_ReScanTimer >= 0.02f) {
            m_ReScanTimer = 0.0f;
            SearchForEnemy(enemies);
        }
    }

    // 防守線拉扯限制：給予寬容的撤退緩衝空間 (195.0f) 避免卡在邊界劇烈抖動
    if (m_TargetEnemy) {
        float distToRally = glm::distance(m_RallyPoint, m_TargetEnemy->GetPosition());
        if (distToRally > 195.0f) {
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
    UpdateAnimation(dt);
}

void Soldier::Draw() {
    if (m_HP <= 0 && m_IsDeadAnimDone) return;

    Util::Transform originalTransform = m_Transform;
    m_Transform.translation.x = std::round(m_Transform.translation.x);
    m_Transform.translation.y = std::round(m_Transform.translation.y);
    m_Transform.scale = glm::vec2(m_FacingRight ? 1.0f : -1.0f, 1.0f);

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
    return (m_CurrentState == State::IDLE || m_CurrentState == State::MOVE_TO_RALLY);
}

void Soldier::ChaseEnemy(float dt) {
    if (!m_TargetEnemy || m_TargetEnemy->GetHP() <= 0 || m_TargetEnemy->ReachedEnd()) {
        ReleaseEnemy();
        SetState(State::MOVE_TO_RALLY);
        return;
    }

    float dist = glm::distance(m_Transform.translation, m_TargetEnemy->GetPosition());

    // 嚴格進入近戰攻擊距離後，才觸發 BLOCKING 阻擋狀態
    if (dist <= m_Config.meleeRange) {
        if (!m_TargetEnemy->IsBlocked()) {
            m_TargetEnemy->SetBlocked(true, shared_from_this());
            m_IsMainBlocker = true;
        } else {
            m_IsMainBlocker = false;
        }
        SetState(State::BLOCKING);
    } else {
        glm::vec2 dir = glm::normalize(m_TargetEnemy->GetPosition() - m_Transform.translation);
        m_Transform.translation += dir * m_Config.speed * (dt * 60.0f);
        m_FacingRight = (dir.x > 0);
    }
}

void Soldier::PerformAttack(float dt) {
    if (!m_TargetEnemy || m_TargetEnemy->GetHP() <= 0 || m_TargetEnemy->ReachedEnd()) {
        ReleaseEnemy();
        SetState(State::MOVE_TO_RALLY);
        return;
    }

    float dist = glm::distance(m_Transform.translation, m_TargetEnemy->GetPosition());

    // 遲滯現象防震盪：允許怪拉開到近戰距離外 + 15.0f 像素，才退回 CHASE 狀態，徹底修正發抖問題
    if (dist > m_Config.meleeRange + 15.0f) {
        ReleaseEnemy();
        SetState(State::CHASE);
        return;
    }

    if (!m_TargetEnemy->IsBlocked()) {
        m_TargetEnemy->SetBlocked(true, shared_from_this());
        m_IsMainBlocker = true;
    }

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
        m_FacingRight = (dir.x > 0);
    } else {
        m_Transform.translation = m_RallyPoint;
        SetState(State::IDLE);
    }
}

void Soldier::SetState(State newState) {
    if (m_CurrentState == newState && newState != State::IDLE) return;

    // 平滑狀態過渡：如果是戰鬥狀態間互切 (CHASE <-> BLOCKING)，不中斷並重置動畫計時器
    bool isCombatTransition = (m_CurrentState == State::CHASE && newState == State::BLOCKING) ||
                              (m_CurrentState == State::BLOCKING && newState == State::CHASE);

    m_CurrentState = newState;

    if (!isCombatTransition) {
        m_AnimTimer = 0.0f;
        m_AttackTimer = 0.0f;
        m_ReScanTimer = 0.0f;

        switch (newState) {
            case State::MOVE_TO_RALLY:
            case State::CHASE:         m_FrameIndex = m_Config.walkStart; break;
            case State::IDLE:          m_FrameIndex = m_Config.walkStart; break;
            case State::BLOCKING:      m_FrameIndex = m_Config.atkStart;  break;
            case State::DEATH:         m_FrameIndex = m_Config.deadStart; break;
        }
        SetDrawable(std::make_shared<Util::Image>(m_Config.spriteRootPath + std::to_string(m_FrameIndex) + ".png"));
    } else {
        if (newState == State::BLOCKING) m_FrameIndex = m_Config.atkStart;
        else if (newState == State::CHASE) m_FrameIndex = m_Config.walkStart;
    }
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

    m_Transform.scale.x = m_FacingRight ? 1.0f : -1.0f;
}

void Soldier::UpdateIdleRotation(float dt) {
    m_TurnTimer += dt;
    if (m_TurnTimer >= m_NextTurnTime) {
        m_TurnTimer = 0.0f;
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
    std::shared_ptr<Enemy> bestTarget = nullptr;
    float alertRange = 160.0f;

    std::vector<std::pair<std::shared_ptr<Enemy>, float>> completelyFreeEnemies;
    std::vector<std::pair<std::shared_ptr<Enemy>, float>> alreadyEngagedEnemies;

    for (auto& enemy : enemies) {
        if (!enemy || enemy->GetHP() <= 0 || enemy->ReachedEnd()) continue;

        float distToMe = glm::distance(m_Transform.translation, enemy->GetPosition());
        float distToRally = glm::distance(m_RallyPoint, enemy->GetPosition());

        // 舊愛最美紅利 (Target Stickiness)：當前目標判定距離扣 30 像素，完美遏制頻繁更換目標引發的抖動
        float scoreDist = distToMe;
        if (enemy == m_TargetEnemy) {
            scoreDist -= 30.0f;
        }

        if (distToMe < alertRange || distToRally < alertRange) {
            if (!enemy->IsBlocked()) {
                completelyFreeEnemies.push_back({enemy, scoreDist});
            } else {
                alreadyEngagedEnemies.push_back({enemy, scoreDist});
            }
        }
    }

    // 分兵與集體圍剿決策優先序
    if (!completelyFreeEnemies.empty()) {
        auto minIt = std::min_element(completelyFreeEnemies.begin(), completelyFreeEnemies.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; });
        bestTarget = minIt->first;
    }
    else if (!alreadyEngagedEnemies.empty()) {
        auto minIt = std::min_element(alreadyEngagedEnemies.begin(), alreadyEngagedEnemies.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; });
        bestTarget = minIt->first;
    }

    if (bestTarget) {
        if (m_TargetEnemy != bestTarget && !m_IsMainBlocker) {
            ReleaseEnemy();
            EngageTarget(bestTarget);
        }
        else if (!m_TargetEnemy) {
            EngageTarget(bestTarget);
        }
    } else {
        if (m_CurrentState == State::CHASE || m_CurrentState == State::BLOCKING) {
            ReleaseEnemy();
            SetState(State::MOVE_TO_RALLY);
        }
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
    m_Transform.scale.x = m_FacingRight ? 1.0f : -1.0f;
}