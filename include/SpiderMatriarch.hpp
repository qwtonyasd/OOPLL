#ifndef SPIDER_MATRIARCH_HPP
#define SPIDER_MATRIARCH_HPP

#include "Enemy.hpp"
#include <vector>
#include <memory>

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
            // 🎯 直接把實時的 enemies 陣列丟進去產卵，完全不需要 m_LevelEnemyList 了！
            SpawnEgg(enemies);
            m_SpawnTimer = 0.0f;
        }
    }

private:
    float m_SpawnTimer = 0.0f;
    float m_SpawnCooldown = 5.0f; // 改為你需要的 5 秒冷卻

    // 🎯 傳入實時安全引用
    void SpawnEgg(std::vector<std::shared_ptr<Enemy>>& enemies);
};

#endif // SPIDER_MATRIARCH_HPP