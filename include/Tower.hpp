#ifndef TOWER_HPP
#define TOWER_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"
#include "Util/Logger.hpp"
#include "Enemy.hpp"
#include <vector>
#include <memory>
#include <limits>

// --- Tower 基類 ---
class Tower : public Util::GameObject {
public:
    enum class Type { ARCHER, MAGE, BARRACKS, BOMB };

    Tower(const glm::vec2& pos, const std::string& imgPath, float range, float cooldown, float damage)
        : m_Range(range), m_Cooldown(cooldown), m_Damage(damage) {

        // --- 核心修正：加入 Y 軸偏移 (Offset) ---
        // 這裡的 25.0f 是微調值，如果塔看起來還是太低，就增加這個數字
        m_Transform.translation = pos + glm::vec2{0.0f, 10.0f};

        SetDrawable(std::make_shared<Util::Image>(imgPath));
        m_ZIndex = 7.0f; // 確保在土堆 (5.0f) 上方，怪物 (10.0f) 下方
    }

    virtual ~Tower() = default;

    virtual void Update(std::vector<std::shared_ptr<Enemy>>& enemies) {
        float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;
        m_Timer += dt;

        if (m_Timer >= m_Cooldown) {
            auto target = FindTarget(enemies);
            if (target) {
                Attack(target, enemies);
                m_Timer = 0.0f;
            }
        }
    }

    virtual void Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) = 0;

protected:
    std::shared_ptr<Enemy> FindTarget(const std::vector<std::shared_ptr<Enemy>>& enemies) {
        std::shared_ptr<Enemy> closest = nullptr;
        float minDistance = m_Range;

        for (const auto& enemy : enemies) {
            float dist = glm::distance(m_Transform.translation, enemy->GetTransform().translation);
            if (dist < minDistance) {
                minDistance = dist;
                closest = enemy;
            }
        }
        return closest;
    }

    float m_Range;
    float m_Cooldown;
    float m_Damage;
    float m_Timer = 0.0f;
};

// --- 1. 弓箭塔 (ArcherTower) ---
class ArcherTower : public Tower {
public:
    ArcherTower(glm::vec2 pos)
        : Tower(pos, "../PTSD/assets/sprites/images/ArcherTower/TowerLevel1/1.png", 250.0f, 0.7f, 15.0f) {}

    void Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) override {
        (void)allEnemies;
        LOG_INFO("Archer Tower fires!");
        // target->DealDamage(m_Damage);
    }
};

// --- 2. 魔法塔 (MageTower) ---
class MageTower : public Tower {
public:
    MageTower(glm::vec2 pos)
        : Tower(pos, "../PTSD/assets/sprites/images/MageTower/TowerLevel1/1.png", 200.0f, 1.5f, 40.0f) {}

    void Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) override {
        (void)allEnemies;
        LOG_INFO("Mage Tower casts magic!");
    }
};

// --- 3. 炸彈塔 (BombTower) ---
class BombTower : public Tower {
public:
    BombTower(glm::vec2 pos)
        : Tower(pos, "../PTSD/assets/sprites/images/BombTower/TowerLevel1/1.png", 180.0f, 2.5f, 50.0f) {}

    void Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) override {
        LOG_INFO("Bomb Tower explosion!");
        glm::vec2 explosionPos = target->GetTransform().translation;
        float explosionRadius = 100.0f;
        for (auto& enemy : allEnemies) {
            if (glm::distance(explosionPos, enemy->GetTransform().translation) <= explosionRadius) {
                // enemy->DealDamage(m_Damage);
            }
        }
    }
};

// --- 4. 兵營 (Barracks) ---
class Barracks : public Tower {
public:
    Barracks(glm::vec2 pos)
        : Tower(pos, "../PTSD/assets/sprites/images/BarracksTower/TowerLevel1/1.png", 150.0f, 4.0f, 5.0f) {}

    void Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) override {
        (void)allEnemies; (void)target;
        LOG_INFO("Barracks blocking enemies!");
    }
};

#endif