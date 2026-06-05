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
    // 1. 處理死亡與特殊狀態
    if (m_HP <= 0 && m_CurrentState != State::DEATH) {
        OnDeath();
        return;
    }
    if (m_CurrentState == State::DEATH || m_CurrentState == State::SKILL) return;

    // 2. 處理攻擊狀態
    if (m_IsBlocked) {
        // 如果剛進入攻擊狀態，切換動畫並執行一次性轉向
        if (m_CurrentState != State::ATTACK) {
            SetState(State::ATTACK);
            SetDrawable(m_AttackAni);
            m_AttackAni->Play();

            // 在切換攻擊瞬間，判斷小兵位置來決定面向
            if (m_TargetSoldier) {
                float direction = (m_TargetSoldier->m_Transform.translation.x < m_Transform.translation.x) ? -1.0f : 1.0f;
                m_Transform.scale.x = direction;
            }
        }

        // 執行攻擊邏輯
        if (m_TargetSoldier && m_TargetSoldier->GetHP() > 0) {
            m_AttackTimer += dt;
            if (m_AttackTimer >= m_AttackCooldown) {
                m_TargetSoldier->TakeDamage(5.0f);
                m_AttackTimer = 0.0f;
            }
        } else {
            // 目標消失，取消阻塞
            m_IsBlocked = false;
        }
    }
    // 3. 處理移動狀態
    else {
        // 若之前是攻擊狀態，恢復移動時重置 scale 為正面
        if (m_CurrentState == State::ATTACK) {
            m_Transform.scale.x = 1.0f;
        }

        MoveTowardsNextNode();

        // 更新移動動畫
        glm::vec2 nextTarget = (m_CurrentNodeIdx < m_Path.size()) ? m_Path[m_CurrentNodeIdx] : m_Transform.translation;
        m_MoveDirection = nextTarget - m_Transform.translation;
        UpdateDirection(m_MoveDirection);
    }
}

void Enemy::MoveTowardsNextNode() {
    if (m_CurrentNodeIdx >= m_Path.size()) return;
    float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;
    glm::vec2 targetPos = m_Path[m_CurrentNodeIdx];
    m_MoveDirection = targetPos - m_Transform.translation;

    float dist = glm::length(m_MoveDirection);
    if (dist > 1.0f) {
        m_Transform.translation += glm::normalize(m_MoveDirection) * m_Speed * dt;
    } else {
        m_CurrentNodeIdx++;
    }
}

void Enemy::Draw() {
    if (m_HP <= 0 && IsDeadAnimationFinished()) return;
    // GameObject::Draw() 內部會呼叫 m_Drawable->Draw()，這將觸發 Animation::Draw()
    // 進而執行自動的 Update() 更新影格
    GameObject::Draw();
    DrawHealthBar();
}

void Enemy::UpdateDirection(glm::vec2 dir) {
    // 若正在攻擊中，不應因為方向微變而中斷攻擊動畫
    if (m_CurrentState == State::ATTACK) return;

    if (std::abs(dir.x) > std::abs(dir.y)) {
        // 水平移動
        SetState(State::MOVE_RIGHT);
        SetDrawable(m_MoveRightAni);

        if (dir.x > 0) {
            // 往右走：將圖片水平翻轉 (因為圖片預設朝左)
            m_Transform.scale.x = 1.0f;
        } else {
            // 往左走：圖片保持預設方向 (朝左)
            m_Transform.scale.x = -1.0f;
        }
    } else {
        // 上下移動時，重置翻轉狀態
        m_Transform.scale.x = 1.0f;

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