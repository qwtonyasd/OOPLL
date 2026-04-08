#include "Soldier.hpp"
#include "Util/Time.hpp"
#include "Util/Image.hpp"

// 修正建構子：只拿 4 個參數
Soldier::Soldier(glm::vec2 spawnPos, glm::vec2 targetPos, float speed, float hp)
    : Unit({spawnPos}, speed, hp), m_RallyPoint(targetPos) {

    m_Transform.translation = spawnPos;
    m_ZIndex = 15.0f;
    m_HP = hp;

    // 路徑初始化
    std::string base = "../PTSD/assets/sprites/images/BarracksTower/Soldier/";
    for(int i=1; i<=6; i++) m_WalkPaths.push_back(base + std::to_string(i) + ".png");
    for(int i=7; i<=17; i++) m_AttackPaths.push_back(base + std::to_string(i) + ".png");
    for(int i=18; i<=24; i++) m_DeadPaths.push_back(base + std::to_string(i) + ".png");

    SetDrawable(std::make_shared<Util::Image>(m_WalkPaths[0]));
}

// 實作父類別要求的 Update
void Soldier::Update() {
    // 留空或放簡單邏輯，因為主要邏輯在 UpdateLogic
}

void Soldier::UpdateLogic(std::vector<std::shared_ptr<Enemy>>& enemies) {
    float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;

    if (m_HP <= 0) {
        m_CurrentState = SoldierState::DEAD;
        UpdateAnimation(dt);
        return;
    }

    // 這裡放你之前的 switch (m_CurrentState) 邏輯 ...
    // 注意：要把 SoldierState::MOVE_TO_RALLY 改回 SoldierState::MOVE (依據你的 enum)
    switch (m_CurrentState) {
        case SoldierState::MOVE:
            MoveTo(m_RallyPoint);
            if (glm::distance(m_Transform.translation, m_RallyPoint) < 5.0f) m_CurrentState = SoldierState::SEARCH;
            break;
        case SoldierState::SEARCH:
            for (auto& enemy : enemies) {
                if (!enemy->IsBlocked() && glm::distance(m_Transform.translation, enemy->GetPosition()) < 100.0f) {
                    m_TargetEnemy = enemy;
                    m_CurrentState = SoldierState::CHASE;
                    break;
                }
            }
            break;
        case SoldierState::CHASE:
            if (!m_TargetEnemy || m_TargetEnemy->GetHP() <= 0) {
                m_CurrentState = SoldierState::MOVE;
                break;
            }
            MoveTo(m_TargetEnemy->GetPosition());
            if (glm::distance(m_Transform.translation, m_TargetEnemy->GetPosition()) < 25.0f) {
                m_TargetEnemy->SetBlocked(true);
                m_CurrentState = SoldierState::ATTACK;
            }
            break;
        case SoldierState::ATTACK:
            if (!m_TargetEnemy || m_TargetEnemy->GetHP() <= 0) {
                m_CurrentState = SoldierState::SEARCH;
                break;
            }
            m_HP -= 5.0f * dt; // 敵人打我
            if (m_AnimIndex == 5) m_TargetEnemy->TakeDamage(10.0f * dt); // 我打敵人
            break;
        default: break;
    }
    UpdateAnimation(dt);
}

void Soldier::MoveTo(glm::vec2 target) {
    glm::vec2 dir = target - m_Transform.translation;
    if (glm::length(dir) > 0.1f) {
        glm::vec2 normDir = glm::normalize(dir);
        m_Transform.translation += normDir * m_Speed;
        m_Transform.scale.x = (normDir.x > 0) ? 1.0f : -1.0f;
    }
}

void Soldier::UpdateAnimation(float dt) {
    m_AnimTimer += dt;
    if (m_AnimTimer > 0.1f) {
        m_AnimTimer = 0;
        m_AnimIndex++;
        // 動畫切換邏輯 ... (同前一個回答)
    }
}

void Soldier::Draw() {
    GameObject::Draw(); // 呼叫 PTSD 的繪製
}