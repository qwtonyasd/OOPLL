#include "tower/Barracks.h"
#include "Util/Time.hpp"

Barracks::Barracks(glm::vec2 pos)
    : Tower(pos, "../PTSD/assets/sprites/images/BarracksTower/TowerLevel1/1.png", 150.0f, 1.0f, 0.0f, 70),
      m_RespawnTimer(0.0f) {
    // 生成三名士兵
    m_Soldiers.push_back(std::make_shared<Soldier>(pos + glm::vec2{0, -40}));
    m_Soldiers.push_back(std::make_shared<Soldier>(pos + glm::vec2{-30, -60}));
    m_Soldiers.push_back(std::make_shared<Soldier>(pos + glm::vec2{30, -60}));
}

void Barracks::Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) {
    (void)target; (void)allEnemies;
}

void Barracks::Update(std::vector<std::shared_ptr<Enemy>>& enemies) {
    float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;
    bool anySoldierDead = false;

    for (auto& enemy : enemies) enemy->SetBlocked(false);

    for (auto& s : m_Soldiers) {
        if (s->IsDead()) {
            anySoldierDead = true;
            s->Update(dt, nullptr);
            continue;
        }

        std::shared_ptr<Enemy> myTarget = nullptr;
        for (auto& enemy : enemies) {
            float dist = glm::distance(s->GetTransform().translation, enemy->GetTransform().translation);
            if (dist <= 45.0f && !enemy->IsBlocked()) {
                enemy->SetBlocked(true);
                myTarget = enemy;
                break;
            }
        }
        s->Update(dt, myTarget);
        if (myTarget) s->TakeDamage(8.0f * dt);
    }

    if (anySoldierDead) {
        m_RespawnTimer += dt;
        if (m_RespawnTimer >= 10.0f) {
            for (auto& s : m_Soldiers) if (s->IsDead()) s->Respawn();
            m_RespawnTimer = 0.0f;
        }
    }
}

void Barracks::Draw() {
    Tower::Draw(); // 畫塔本身
    for (auto& s : m_Soldiers) s->Draw(); // 畫士兵
}