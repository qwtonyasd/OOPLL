#include "tower/Barracks.h"
#include "Util/Time.hpp"

Barracks::Barracks(glm::vec2 pos, const std::vector<glm::vec2>& route)
    : Tower(pos, "../PTSD/assets/sprites/images/BarracksTower/TowerLevel1/1.png", 0.0f, 10.0f, 0.0f, 70),
      m_Route(route) {
    for (int i = 0; i < m_MaxSoldiers; ++i) SpawnSoldier();
}

void Barracks::Update(std::vector<std::shared_ptr<Enemy>>& enemies,
                      std::vector<std::shared_ptr<Projectile>>& projectiles) {
    (void)projectiles;
    float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;

    // 自動補兵
    if (m_Soldiers.size() < (size_t)m_MaxSoldiers) {
        m_SpawnTimer += dt;
        if (m_SpawnTimer >= 10.0f) {
            SpawnSoldier();
            m_SpawnTimer = 0.0f;
        }
    }

    // 更新士兵與清理死亡士兵
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
    glm::vec2 offset;
    int id = m_Soldiers.size();
    if (id == 0)      offset = {0, -45};
    else if (id == 1) offset = {-25, -70};
    else              offset = {25, -70};

    auto s = std::make_shared<Soldier>(m_Transform.translation, m_Transform.translation + offset, 1.3f, 100.0f);
    m_Soldiers.push_back(s);
}

void Barracks::Draw() {
    Tower::Draw();
    for (auto& s : m_Soldiers) s->Draw();
}

void Barracks::Attack(std::shared_ptr<Enemy> t, std::vector<std::shared_ptr<Enemy>>& e, std::vector<std::shared_ptr<Projectile>>& p) {
    (void)t; (void)e; (void)p;
}