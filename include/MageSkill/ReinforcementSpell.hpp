#pragma once
#include "Spell.hpp"
#include "Soldier.hpp"
#include <vector>
#include <memory>
#include <iostream>

class ReinforcementSoldier : public Soldier {
private:
    float m_Lifetime;
public:
    ReinforcementSoldier(glm::vec2 spawnPos, glm::vec2 rallyPoint, const SoldierConfig& config, float lifetime = 15.0f)
        : Soldier(spawnPos, rallyPoint, config), m_Lifetime(lifetime) {}

    void Update(std::vector<std::shared_ptr<Enemy>>& enemies, float dt) override {
        if (m_HP > 0) {
            m_Lifetime -= dt;
            if (m_Lifetime <= 0.0f) m_HP = 0;
        }
        Soldier::Update(enemies, dt);
    }
};

class ReinforcementSpell : public Spell {
private:
    std::vector<std::shared_ptr<Soldier>>& m_StageSoldiers;

public:
    ReinforcementSpell(std::vector<std::shared_ptr<Soldier>>& stageSoldiers)
        : Spell(10.0f, "../PTSD/assets/sprites/images/Skill/Summon/1.png"),
          m_StageSoldiers(stageSoldiers) {}

    void Cast(glm::vec2 clickPos) override {
        if (!IsReady()) return;

        Soldier::SoldierConfig peasantConfig;
        // 這是你農夫圖片所在的正確路徑
        peasantConfig.spriteRootPath = "../PTSD/assets/sprites/images/Skill/Summon/Soldier/";

        peasantConfig.walkStart = 1;  peasantConfig.walkEnd = 5;
        peasantConfig.atkStart = 7;   peasantConfig.atkEnd = 9;
        peasantConfig.deadStart = 10; peasantConfig.deadEnd = 13;

        peasantConfig.maxHP = 30.0f;
        peasantConfig.speed = 1.6f;
        peasantConfig.minDamage = 4;
        peasantConfig.maxDamage = 8;
        peasantConfig.attackCooldown = 1.0f;
        peasantConfig.detectionRange = 80.0f;
        peasantConfig.meleeRange = 20.0f;

        std::cout << "[DEBUG] Spawning at: " << peasantConfig.spriteRootPath << std::endl;

        auto p1 = std::make_shared<ReinforcementSoldier>(clickPos + glm::vec2(-15, 0), clickPos + glm::vec2(-15, 0), peasantConfig, 15.0f);
        auto p2 = std::make_shared<ReinforcementSoldier>(clickPos + glm::vec2(15, 0), clickPos + glm::vec2(15, 0), peasantConfig, 15.0f);

        m_StageSoldiers.push_back(p1);
        m_StageSoldiers.push_back(p2);

        StartCooldown();
    }
};