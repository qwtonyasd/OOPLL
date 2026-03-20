#ifndef ENEMY_FACTORY_HPP
#define ENEMY_FACTORY_HPP

#include "Enemy.hpp"

class EnemyFactory {
public:
    enum class Type { GOBLIN, SLIME };

    static std::shared_ptr<Enemy> Create(Type type, const std::vector<glm::vec2>& path) {
        switch (type) {
            case Type::GOBLIN:
                // 哥布林：速度 200.0f，血量 50.0f
                return std::make_shared<Enemy>(path, 200.0f, 50.0f, "../PTSD/assets/sprites/images/Enemy/1.png");
            case Type::SLIME:
                // 史萊姆：速度 80.0f (較慢)，血量 100.0f
                return std::make_shared<Enemy>(path, 80.0f, 100.0f, "../assets/slime.png");
        }
        return nullptr;
    }
};

#endif