#ifndef ENEMY_FACTORY_HPP
#define ENEMY_FACTORY_HPP

#include "Enemy.hpp"
#include "Shaman.hpp"
#include "SpiderMatriarch.hpp"
#include "Egg.hpp"
#include <filesystem>
#include <vector>
#include <string>
#include <memory>

class EnemyFactory {
public:
    static std::vector<std::string> generatePaths(const std::string& folder, int start, int end) {
        std::vector<std::string> paths;
        for (int i = start; i <= end; ++i) {
            std::string fullPath = "../PTSD/assets/sprites/images/Enemy/" + folder + "/" + std::to_string(i) + ".png";
            if (std::filesystem::exists(fullPath)) {
                paths.push_back(fullPath);
            }
        }
        return paths;
    }

    static std::vector<std::string> generateStepPaths(const std::string& folder, int start, int end, int step) {
        std::vector<std::string> paths;
        for (int i = start; i <= end; i += step) {
            std::string fullPath = "../PTSD/assets/sprites/images/Enemy/" + folder + "/" + std::to_string(i) + ".png";
            if (std::filesystem::exists(fullPath)) {
                paths.push_back(fullPath);
            }
        }
        return paths;
    }

    static std::shared_ptr<Enemy> CreateEgg(const std::vector<glm::vec2>& remainingPath, const glm::vec2& spawnPos) {
        std::vector<std::vector<std::string>> spiderMoveAnis = {
            generateStepPaths("Spider-Matriarch/egg", 12201, 12217, 2),
            generateStepPaths("Spider-Matriarch/egg", 12219, 12235, 2),
            generateStepPaths("Spider-Matriarch/egg", 12237, 12253, 2)
        };

        std::vector<std::string> spiderAttack = generateStepPaths("Spider-Matriarch/egg", 12255, 12281, 2);
        std::vector<std::string> spiderDead   = generateStepPaths("Spider-Matriarch/egg", 12309, 12319, 2);
        std::vector<std::string> eggHatch    = generateStepPaths("Spider-Matriarch/egg", 10569, 10667, 2);

        // 蛋繼承自 Enemy，數值設定（盾：無）
        auto egg = std::make_shared<Egg>(
            Enemy::Type::EGG, remainingPath, 85.0f, 15.0f,
            spiderMoveAnis, spiderAttack, spiderDead, eggHatch, spawnPos
        );
        egg->SetResistances(Enemy::DefenseLevel::NONE, Enemy::DefenseLevel::NONE);
        return egg;
    }

    static std::shared_ptr<Enemy> Create(Enemy::Type type, const std::vector<glm::vec2>& path) {
        if (type == Enemy::Type::GOBLIN) {
            std::vector<std::vector<std::string>> moveAnis = {
                generatePaths("Goblin", 15748, 15762),
                generatePaths("Goblin", 15788, 15810),
                generatePaths("Goblin", 15764, 15786)
            };
            auto enemy = std::make_shared<Enemy>(type, path, 45.0f, 20.0f, moveAnis,
                generatePaths("Goblin", 15814, 15828), generatePaths("Goblin", 15830, 15865));
            enemy->SetResistances(Enemy::DefenseLevel::NONE, Enemy::DefenseLevel::NONE);
            return enemy;
        }
        else if (type == Enemy::Type::ORC) {
            std::vector<std::vector<std::string>> moveAnis = {
                generatePaths("Orc", 14886, 14908),
                generatePaths("Orc", 14934, 14956),
                generatePaths("Orc", 14910, 14932)
            };
            auto enemy = std::make_shared<Enemy>(type, path, 50.0f, 80.0f, moveAnis,
                generatePaths("Orc", 14958, 14968), generatePaths("Orc", 14970, 15004));
            enemy->SetResistances(Enemy::DefenseLevel::NONE, Enemy::DefenseLevel::NONE);
            return enemy;
        }
        else if (type == Enemy::Type::WORG) {
            std::vector<std::vector<std::string>> moveAnis = {
                generateStepPaths("Worg", 11148, 11162, 2),
                generateStepPaths("Worg", 11174, 11182, 2),
                generateStepPaths("Worg", 11164, 11172, 2)
            };
            auto enemy = std::make_shared<Enemy>(type, path, 80.0f, 120.0f, moveAnis,
                generateStepPaths("Worg", 11184, 11202, 2), generatePaths("Worg", 11227, 11241));
            // 🎯 Worg: Magic Resistance Medium
            enemy->SetResistances(Enemy::DefenseLevel::NONE, Enemy::DefenseLevel::MEDIUM);
            return enemy;
        }
        else if (type == Enemy::Type::WULF) {
            std::vector<std::vector<std::string>> moveAnis = {
                generatePaths("Wulf", 1, 8),
                generatePaths("Wulf", 14, 18),
                generatePaths("Wulf", 9, 13)
            };
            auto enemy = std::make_shared<Enemy>(type, path, 80.0f, 35.0f, moveAnis,
                generatePaths("Shaman", 1, 12), generatePaths("Wulf", 29, 37));
            enemy->SetResistances(Enemy::DefenseLevel::NONE, Enemy::DefenseLevel::NONE);
            return enemy;
        }
        else if (type == Enemy::Type::SHAMAN) {
            std::vector<std::vector<std::string>> moveAnis = {
                generatePaths("Shaman", 1, 12),
                generatePaths("Shaman", 25, 36),
                generatePaths("Shaman", 13, 24)
            };
            std::vector<std::string> skillEffectPaths = generateStepPaths("Shaman", 10815, 10863, 2);

            auto enemy = std::make_shared<Shaman>(type, path, 35.0f, 100.0f, moveAnis,
                generatePaths("Shaman", 37, 44), generatePaths("Shaman", 80, 84),
                generatePaths("Shaman", 45, 67), skillEffectPaths);
            // 🎯 Shaman: Magic Resistance High
            enemy->SetResistances(Enemy::DefenseLevel::NONE, Enemy::DefenseLevel::HIGH);
            return enemy;
        }
        else if (type == Enemy::Type::OGRE) {
            std::vector<std::vector<std::string>> moveAnis = {
                generateStepPaths("Ogre", 15008, 15034, 2),
                generateStepPaths("Ogre", 15062, 15086, 2),
                generateStepPaths("Ogre", 15036, 15060, 2)
            };
            auto enemy = std::make_shared<Enemy>(type, path, 30.0f, 800.0f, moveAnis,
                generateStepPaths("Ogre", 15088, 15108, 2), generateStepPaths("Ogre", 15136, 15154, 2));
            enemy->SetResistances(Enemy::DefenseLevel::NONE, Enemy::DefenseLevel::NONE);
            return enemy;
        }
        else if (type == Enemy::Type::BANDIT) {
            std::vector<std::vector<std::string>> moveAnis = {
                generateStepPaths("Bandit", 18433, 18455, 2),
                generateStepPaths("Bandit", 18481, 18503, 2),
                generateStepPaths("Bandit", 18457, 18479, 2)
            };
            auto enemy = std::make_shared<Enemy>(type, path, 55.0f, 70.0f, moveAnis,
                generateStepPaths("Bandit", 18505, 18509, 2), generateStepPaths("Bandit", 18535, 18547, 2));
            enemy->SetResistances(Enemy::DefenseLevel::NONE, Enemy::DefenseLevel::NONE);
            return enemy;
        }
        else if (type == Enemy::Type::BRIGAND) {
            std::vector<std::vector<std::string>> moveAnis = {
                generateStepPaths("Brigand", 16972, 16994, 2),
                generateStepPaths("Brigand", 17020, 17042, 2),
                generateStepPaths("Brigand", 16996, 17018, 2)
            };
            auto enemy = std::make_shared<Enemy>(type, path, 40.0f, 160.0f, moveAnis,
                generateStepPaths("Brigand", 17044, 17056, 2), generateStepPaths("Brigand", 17082, 17090, 2));
            // 🎯 Brigand: Armor Rating Medium
            enemy->SetResistances(Enemy::DefenseLevel::MEDIUM, Enemy::DefenseLevel::NONE);
            return enemy;
        }
        else if (type == Enemy::Type::MARAUDER) {
            std::vector<std::vector<std::string>> moveAnis = {
                generateStepPaths("Marauder", 15346, 15368, 2),
                generateStepPaths("Marauder", 15394, 15416, 2),
                generateStepPaths("Marauder", 15370, 15392, 2)
            };
            auto enemy = std::make_shared<Enemy>(type, path, 42.0f, 600.0f, moveAnis,
                generateStepPaths("Marauder", 15418, 15432, 2), generateStepPaths("Marauder", 15458, 15478, 2));
            // 🎯 Marauder: Armor Rating Medium
            enemy->SetResistances(Enemy::DefenseLevel::MEDIUM, Enemy::DefenseLevel::NONE);
            return enemy;
        }
        else if (type == Enemy::Type::GIANT_SPIDER) {
            std::vector<std::vector<std::string>> moveAnis = {
                generateStepPaths("Giant-Spider", 12451, 12473, 2),
                generateStepPaths("Giant-Spider", 12487, 12503, 2),
                generateStepPaths("Giant-Spider", 12475, 12485, 2)
            };
            auto enemy = std::make_shared<Enemy>(type, path, 70.0f, 60.0f, moveAnis,
                generateStepPaths("Giant-Spider", 12505, 12531, 2), generateStepPaths("Giant-Spider", 12559, 12569, 2));
            // 🎯 Giant-Spider: Magic Resistance High
            enemy->SetResistances(Enemy::DefenseLevel::NONE, Enemy::DefenseLevel::HIGH);
            return enemy;
        }
        else if (type == Enemy::Type::SPIDER_MATRIARCH) {
            std::vector<std::vector<std::string>> moveAnis = {
                generateStepPaths("Spider-Matriarch", 12580, 12596, 2),
                generateStepPaths("Spider-Matriarch", 12616, 12632, 2),
                generateStepPaths("Spider-Matriarch", 12598 , 12614, 2)
            };

            auto enemy = std::make_shared<SpiderMatriarch>(type, path, 28.0f, 250.0f, moveAnis,
                generateStepPaths("Spider-Matriarch", 12634, 12652, 2), generateStepPaths("Spider-Matriarch", 12678, 12694, 2));
            // 🎯 Spider-Matriarch: Magic Resistance High
            enemy->SetResistances(Enemy::DefenseLevel::NONE, Enemy::DefenseLevel::HIGH);
            return enemy;
        }
        else if (type == Enemy::Type::SPIDERLING) {
            std::vector<std::vector<std::string>> moveAnis = {
                generateStepPaths("Spider-Matriarch/egg", 12201, 12217, 2),
                generateStepPaths("Spider-Matriarch/egg", 12219, 12235, 2),
                generateStepPaths("Spider-Matriarch/egg", 12237, 12253, 2)
            };

            auto enemy = std::make_shared<Enemy>(type, path, 85.0f, 30.0f, moveAnis,
                generateStepPaths("Spider-Matriarch/egg", 12255, 12281, 2), generateStepPaths("Spider-Matriarch/egg", 12309, 12319, 2));
            enemy->SetResistances(Enemy::DefenseLevel::NONE, Enemy::DefenseLevel::NONE);
            return enemy;
        }
        return nullptr;
    }
};

#endif // ENEMY_FACTORY_HPP