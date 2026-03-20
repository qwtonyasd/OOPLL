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
        m_Transform.translation = pos;
        SetDrawable(std::make_shared<Util::Image>(imgPath));
        m_ZIndex = 7.0f;
    }

    virtual ~Tower() = default;

    // 核心邏輯：每一幀檢查是否可以攻擊
    virtual void Update(std::vector<std::shared_ptr<Enemy>>& enemies) {
        float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;
        m_Timer += dt;

        if (m_Timer >= m_Cooldown) {
            auto target = FindTarget(enemies);
            if (target) {
                Attack(target, enemies); // 執行攻擊
                m_Timer = 0.0f;         // 重設冷卻
            }
        }
    }

    // 不同的塔有不同的攻擊方式
    virtual void Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) = 0;

protected:
    // 尋找範圍內最近的敵人
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

// --- 1. 弓箭塔 (ArcherTower): 單體快速射擊 ---
class ArcherTower : public Tower {
public:
    ArcherTower(glm::vec2 pos)
        : Tower(pos, "../PTSD/assets/sprites/images/ArcherTower/TowerLevel1/1.png", 250.0f, 0.7f, 15.0f) {}

    void Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) override {
        (void)allEnemies;
        LOG_INFO("Archer Tower fires an arrow at enemy!");
        // target->DealDamage(m_Damage); // 假設 Enemy 有 DealDamage 函式
    }
};

// --- 2. 魔法塔 (MageTower): 無視部分護甲或持續傷害 ---
class MageTower : public Tower {
public:
    MageTower(glm::vec2 pos)
        : Tower(pos, "../PTSD/assets/sprites/images/MageTower/TowerLevel1/1.png", 200.0f, 1.5f, 40.0f) {}

    void Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) override {
        (void)allEnemies;
        LOG_INFO("Mage Tower casts a magic bolt!");
        // target->DealDamage(m_Damage);
    }
};

// --- 3. 炸彈塔 (BombTower): 範圍傷害 (AOE) ---
class BombTower : public Tower {
public:
    BombTower(glm::vec2 pos)
        : Tower(pos, "../PTSD/assets/sprites/images/BombTower/TowerLevel1/1.png", 180.0f, 2.5f, 50.0f) {}

    void Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) override {
        LOG_INFO("Bomb Tower performs AOE explosion!");
        glm::vec2 explosionPos = target->GetTransform().translation;
        float explosionRadius = 100.0f;

        for (auto& enemy : allEnemies) {
            if (glm::distance(explosionPos, enemy->GetTransform().translation) <= explosionRadius) {
                // enemy->DealDamage(m_Damage); // 範圍內所有人都受傷
            }
        }
    }
};

// --- 4. 兵營 (Barracks): 產兵攔截 (目前簡化為減速攻擊) ---
class Barracks : public Tower {
public:
    Barracks(glm::vec2 pos)
        : Tower(pos, "../PTSD/assets/sprites/images/BarracksTower/TowerLevel1/1.png", 150.0f, 4.0f, 5.0f) {}

    void Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) override {
        (void)allEnemies; (void)target;
        LOG_INFO("Barracks soldiers are blocking the path!");
        // 這裡未來可以實作生成「Soldier」物件在地圖上
    }
};

#endif