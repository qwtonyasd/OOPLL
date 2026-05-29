#ifndef SPIDER_MATRIARCH_HPP
#define SPIDER_MATRIARCH_HPP

#include "Enemy.hpp"
#include <vector>
#include <memory>

// 前置宣告
class EnemyFactory;

class SpiderMatriarch : public Enemy {
public:
    SpiderMatriarch(Enemy::Type type, const std::vector<glm::vec2>& path, float speed, float hp,
                    const std::vector<std::vector<std::string>>& moveAnimations,
                    const std::vector<std::string>& attackPaths,
                    const std::vector<std::string>& deadPaths)
        : Enemy(type, path, speed, hp, moveAnimations, attackPaths, deadPaths) {}

    void Update(std::vector<std::shared_ptr<Enemy>>& enemies, float dt) override {
        Enemy::Update(enemies, dt);

        if (m_CurrentState == State::DEATH) return;

        m_SpawnTimer += dt;
        if (m_SpawnTimer >= m_SpawnCooldown) {
            SpawnEgg();
            m_SpawnTimer = 0.0f;
        }
    }

    void SetLevelEnemyList(std::vector<std::shared_ptr<Enemy>>* enemyList) {
        m_LevelEnemyList = enemyList;
    }

private:
    float m_SpawnTimer = 0.0f;
    float m_SpawnCooldown = 8.0f;
    std::vector<std::shared_ptr<Enemy>>* m_LevelEnemyList = nullptr;

    // 僅宣告，不在此實作
    void SpawnEgg();
};

#endif // SPIDER_MATRIARCH_HPP