#include "tower/Barracks.h"
#include "Util/Time.hpp"
#include "MapData.hpp"

Barracks::Barracks(glm::vec2 pos, const std::vector<glm::vec2>& [[maybe_unused]] route)
    : Tower(pos, "../PTSD/assets/sprites/images/BarracksTower/TowerLevel1/1.png",
            0.0f, 10.0f, 0.0f, 70, Enemy::DamageType::PHYSICAL), m_Route(route) {
    for (int i = 0; i < m_MaxSoldiers; ++i) SpawnSoldier();
    m_VisualOffset = 10.0f;
}

void Barracks::Update(std::vector<std::shared_ptr<Enemy>>& enemies,
                      std::vector<std::shared_ptr<Projectile>>& [[maybe_unused]] projectiles) {
    float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;

    if (m_Soldiers.size() < (size_t)m_MaxSoldiers) {
        m_SpawnTimer += dt;
        if (m_SpawnTimer >= 10.0f) {
            SpawnSoldier();
            m_SpawnTimer = 0.0f;
        }
    }

    for (auto& s : m_Soldiers) s->Update(enemies);
    for (auto it = m_Soldiers.begin(); it != m_Soldiers.end(); ) {
        if ((*it)->GetHP() <= 0 && (*it)->IsDeadAnimationFinished()) {
            (*it)->ReleaseEnemy();
            it = m_Soldiers.erase(it);
        } else {
            ++it;
        }
    }
}

void Barracks::SpawnSoldier() {
    // 這裡呼叫 MapData::GetBaseRallyPoint 必須與標頭檔定義完全一致
    glm::vec2 baseRallyPoint = MapData::GetBaseRallyPoint(m_Transform.translation);

    glm::vec2 formationOffset;
    int id = m_Soldiers.size();
    if (id == 0)      formationOffset = {0, 0};
    else if (id == 1) formationOffset = {-20, -15};
    else              formationOffset = {20, -15};

    glm::vec2 finalTarget = baseRallyPoint + formationOffset;

    auto s = std::make_shared<Soldier>(
        m_Transform.translation,
        finalTarget,
        1.5f,
        50.0f
    );
    m_Soldiers.push_back(s);
}

void Barracks::Draw() {
    Tower::Draw();
    for (auto& s : m_Soldiers) s->Draw();
}

void Barracks::Attack([[maybe_unused]] std::shared_ptr<Enemy> t,
                      [[maybe_unused]] std::vector<std::shared_ptr<Enemy>>& e,
                      [[maybe_unused]] std::vector<std::shared_ptr<Projectile>>& p) {
    // 兵營不主動攻擊
}