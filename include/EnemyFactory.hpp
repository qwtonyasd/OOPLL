#ifndef ENEMY_FACTORY_HPP
#define ENEMY_FACTORY_HPP

#include "Enemy.hpp"
#include "Shaman.hpp" // 引入 Shaman 子類別標頭檔
#include <filesystem>
#include <vector>
#include <string>
#include <memory>

class EnemyFactory {
public:
    static std::shared_ptr<Enemy> Create(Enemy::Type type, const std::vector<glm::vec2>& path) {
        // 輔助函式：連續編輯的檔案名稱
        auto generatePaths = [](const std::string& folder, int start, int end) {
            std::vector<std::string> paths;
            for (int i = start; i <= end; ++i) {
                std::string fullPath = "../PTSD/assets/sprites/images/Enemy/" + folder + "/" + std::to_string(i) + ".png";
                if (std::filesystem::exists(fullPath)) {
                    paths.push_back(fullPath);
                }
            }
            return paths;
        };

        if (type == Enemy::Type::GOBLIN) {
            std::vector<std::vector<std::string>> moveAnis = {
                generatePaths("Goblin", 15748, 15762),
                generatePaths("Goblin", 15764, 15786),
                generatePaths("Goblin", 15788, 15810)
            };
            return std::make_shared<Enemy>(
                type, path, 45.0f, 20.0f, moveAnis,
                generatePaths("Goblin", 15814, 15828),
                generatePaths("Goblin", 15830, 15865)
            );
        }
        else if (type == Enemy::Type::ORC) {
            std::vector<std::vector<std::string>> moveAnis = {
                generatePaths("Orc", 14886, 14908),
                generatePaths("Orc", 14910, 14932),
                generatePaths("Orc", 14934, 14956)
            };
            return std::make_shared<Enemy>(
                type, path, 50.0f, 80.0f, moveAnis,
                generatePaths("Orc", 14958, 14968),
                generatePaths("Orc", 14970, 15004)
            );
        }
        else if (type == Enemy::Type::WULF) {
            std::vector<std::vector<std::string>> moveAnis = {
                generatePaths("Wulf", 1, 8),
                generatePaths("Wulf", 9, 11),
                generatePaths("Wulf", 12, 18)
            };
            return std::make_shared<Enemy>(
                type, path, 70.0f, 40.0f, moveAnis,
                generatePaths("Wulf", 19, 28),
                generatePaths("Wulf", 29, 37)
            );
        }
        else if (type == Enemy::Type::SHAMAN) {
            // Shaman 本體動畫區間設定
            std::vector<std::vector<std::string>> moveAnis = {
                generatePaths("Shaman", 1, 12),
                generatePaths("Shaman", 13, 24),
                generatePaths("Shaman", 25, 36)
            };

            // 特殊處理：技能特效圖片（10815 - 10863 均為奇數）
            std::vector<std::string> skillEffectPaths;
            for (int i = 10815; i <= 10863; i += 2) {
                std::string fullPath = "../PTSD/assets/sprites/images/Enemy/Shaman/skill/" + std::to_string(i) + ".png";
                if (std::filesystem::exists(fullPath)) {
                    skillEffectPaths.push_back(fullPath);
                }
            }

            // 使用 Shaman 的全新建構子（本體施法動作 45-67.png，另外傳入綠色十字特效路徑）
            return std::make_shared<Shaman>(
                type,
                path,
                35.0f,
                60.0f,
                moveAnis,
                generatePaths("Shaman", 37, 44),
                generatePaths("Shaman", 80, 84),
                generatePaths("Shaman", 45, 67), // 薩滿揮舞法杖施法動作
                skillEffectPaths                  // 綠色十字加血特效
            );
        }
        else if (type == Enemy::Type::OGRE) {
            std::vector<std::vector<std::string>> moveAnis = {
                generatePaths("Ogre", 15008, 15034),
                generatePaths("Ogre", 15036, 15060),
                generatePaths("Ogre", 15062, 15086)
            };
            return std::make_shared<Enemy>(
                type, path, 30.0f, 350.0f, moveAnis,
                generatePaths("Ogre", 15088, 15108),
                generatePaths("Ogre", 15136, 15154)
            );
        }
        return nullptr;
    }
};

#endif // ENEMY_FACTORY_HPP