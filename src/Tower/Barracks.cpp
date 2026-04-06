#include "tower/Barracks.h"
#include "Util/Time.hpp"
#include <limits>
#include <algorithm>

// Helper function to get the closest point on a segment
static glm::vec2 GetClosestPointOnSegment(glm::vec2 A, glm::vec2 B, glm::vec2 P) {
    glm::vec2 AB = B - A;
    float l2 = glm::dot(AB, AB);
    if (l2 == 0.0f) return A;
    float t = std::max(0.0f, std::min(1.0f, glm::dot(P - A, AB) / l2));
    return A + t * AB;
}

Barracks::Barracks(glm::vec2 pos, const std::vector<glm::vec2>& route)
    : Tower(pos, "../PTSD/assets/sprites/images/BarracksTower/TowerLevel1/1.png", 150.0f, 1.0f, 0.0f, 70),
      m_RespawnTimer(0.0f) {

    glm::vec2 rallyPoint = pos;
    glm::vec2 segmentDir = {1.0f, 0.0f}; // 預設路徑方向
    float minDistance = std::numeric_limits<float>::max();

    // 1. 尋找最近路段並計算投影點與切線
    if (route.size() >= 2) {
        for (size_t i = 0; i < route.size() - 1; ++i) {
            glm::vec2 p1 = route[i];
            glm::vec2 p2 = route[i + 1];
            glm::vec2 closest = GetClosestPointOnSegment(p1, p2, pos);
            float d = glm::distance(pos, closest);

            if (d < minDistance) {
                minDistance = d;
                rallyPoint = closest;
                segmentDir = glm::normalize(p2 - p1); // 取得路徑方向向量
            }
        }
    }

    // 2. 計算法向量 (確保垂直於路徑的方向)
    glm::vec2 normal = glm::vec2(-segmentDir.y, segmentDir.x);

    // --- 關鍵修正：三角形陣型與指向 ---
    // frontDist 是三角形頂點往前推的距離
    // sideDist 是兩翼縮在後方並左右排開的距離
    float frontDist = 18.0f; // 前方頂點 (隊長) 往前偏移
    float sideDist = 12.0f;  // 左右偏移 (橫向展開)
    float backStep = 5.0f;  // 兩翼縮在前方頂點後面

    m_Soldiers.clear();

    // 隊長：站在投影中心點稍微往前 (順著路徑)
    // 這會確保三角形尖端永遠指著敵人來的方向
    m_Soldiers.push_back(std::make_shared<Soldier>(rallyPoint + segmentDir * frontDist));

    // 左翼：從投影中心點往後退 ( backStep) 並往法向量偏移 ( sideDist)
    m_Soldiers.push_back(std::make_shared<Soldier>(rallyPoint - segmentDir * backStep + normal * sideDist));

    // 右翼：從投影中心點往後退 ( backStep) 並往反法向量偏移 (- sideDist)
    m_Soldiers.push_back(std::make_shared<Soldier>(rallyPoint - segmentDir * backStep - normal * sideDist));
}

void Barracks::Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) {
    (void)target;
    (void)allEnemies;
}

void Barracks::Update(std::vector<std::shared_ptr<Enemy>>& enemies) {
    float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;
    bool anySoldierDead = false;

    for (auto& enemy : enemies) {
        enemy->SetBlocked(false);
    }

    for (auto& s : m_Soldiers) {
        if (s->IsDead()) {
            anySoldierDead = true;
            s->Update(dt, nullptr);
            continue;
        }

        std::shared_ptr<Enemy> myTarget = nullptr;
        for (auto& enemy : enemies) {
            if (!enemy->IsBlocked()) {
                float dist = glm::distance(s->GetTransform().translation, enemy->GetTransform().translation);
                // Reduced block radius to keep them on the road during combat
                if (dist <= 35.0f) {
                    enemy->SetBlocked(true);
                    myTarget = enemy;
                    break;
                }
            }
        }

        s->Update(dt, myTarget);

        if (myTarget) {
            s->TakeDamage(10.0f * dt);
        }
    }

    if (anySoldierDead) {
        m_RespawnTimer += dt;
        if (m_RespawnTimer >= 10.0f) {
            for (auto& s : m_Soldiers) {
                if (s->IsDead()) s->Respawn();
            }
            m_RespawnTimer = 0.0f;
        }
    }
}

void Barracks::Draw() {
    Tower::Draw();
    for (auto& s : m_Soldiers) {
        s->Draw();
    }
}