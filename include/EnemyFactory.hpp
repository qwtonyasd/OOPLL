#ifndef ENEMY_FACTORY_HPP
#define ENEMY_FACTORY_HPP

#include "Enemy.hpp"
#include <filesystem>
#include <vector>
#include <string>
#include <memory>

class EnemyFactory {
public:
    static std::shared_ptr<Enemy> Create(Enemy::Type type, const std::vector<glm::vec2>& path) {
        // 輔助函式：修正為 i++，確保檢查區間內所有可能的檔案名稱
        auto generatePaths = [](const std::string& folder, int start, int end) {
            std::vector<std::string> paths;
            // 改為 i++，因為檔案編號並不全是偶數，且有跳號現象
            for (int i = start; i <= end; ++i) {
                std::string fullPath = "../PTSD/assets/sprites/images/Enemy/" + folder + "/" + std::to_string(i) + ".png";
                if (std::filesystem::exists(fullPath)) {
                    paths.push_back(fullPath);
                }
            }
            return paths;
        };

        if (type == Enemy::Type::GOBLIN) {
            // Goblin 動畫區間設定
            std::vector<std::vector<std::string>> moveAnis = {
                generatePaths("Goblin", 15748, 15762), // 向右
                generatePaths("Goblin", 15764, 15786), // 向上
                generatePaths("Goblin", 15788, 15810)  // 向下
            };

            // 死亡動畫區間 15830 ~ 15865
            // 改用 i++ 後，它會抓到：15830, 15832, 15854, 15856, 15859, 15861, 15863, 15865
            return std::make_shared<Enemy>(path, 75.0f, 60.0f,
                moveAnis,
                generatePaths("Goblin", 15814, 15828), // 攻擊
                generatePaths("Goblin", 15830, 15865)  // 死亡
            );
        }
        else if (type == Enemy::Type::ORC) {
            // Orc 動畫區間設定
            std::vector<std::vector<std::string>> moveAnis = {
                generatePaths("Orc", 14886, 14908), // 向右
                generatePaths("Orc", 14910, 14932), // 向上
                generatePaths("Orc", 14934, 14956)  // 向下
            };

            return std::make_shared<Enemy>(path, 50.0f, 150.0f,
                moveAnis,
                generatePaths("Orc", 14958, 14968), // 攻擊
                generatePaths("Orc", 14970, 15004)  // 死亡
            );
        }
        return nullptr;
    }
};

#endif