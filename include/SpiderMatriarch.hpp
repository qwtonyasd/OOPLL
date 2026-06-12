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
            SpawnEgg(enemies);
            m_SpawnTimer = 0.0f;
        }
    }

private:
    float m_SpawnTimer = 0.0f;
    float m_SpawnCooldown = 5.0f; // 5 秒產卵一次

    void SpawnEgg(std::vector<std::shared_ptr<Enemy>>& enemies);
};

#endif // SPIDER_MATRIARCH_HPP