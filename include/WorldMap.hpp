#ifndef WORLD_MAP_HPP
#define WORLD_MAP_HPP

#include <vector>
#include <memory>
#include <string>
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Transform.hpp"
#include "Util/TransformUtils.hpp"
#include "Util/Time.hpp"
#include "LevelStatus.hpp"
#include <iostream>

class WorldMap {
public:
    WorldMap() {
        m_Background = std::make_shared<Util::Image>("../PTSD/assets/sprites/images/start/1.png");
        m_UpgradeImage = std::make_shared<Util::Image>("../PTSD/assets/sprites/images/Upgrade/4736.png");
        m_StarImage = std::make_shared<Util::Image>("../PTSD/assets/sprites/images/Start/13.png");

        // 預載 1 到 116 幀的旗幟動畫資源
        m_FlagFrames.resize(117);
        for (int i = 1; i <= 116; ++i) {
            std::string path = "../PTSD/assets/sprites/images/Flag/" + std::to_string(i) + ".png";
            m_FlagFrames[i] = std::make_shared<Util::Image>(path);
        }

        m_LevelPositions = {
            {-240, -170}, {-120, 40}, {140, -80}, {170, 110}, {160, 170}
        };

        m_UpgradeButtonPos = {280, -210};
    }

    // 當回到大地圖時，由外部呼叫此函式重置時間，讓動畫重新播放
    void ResetAnimation() {
        m_AnimationTimer = 0.0f;
    }

    int Update(const std::vector<LevelStatus>& levels) {
        m_AnimationTimer += static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;

        Draw(levels);
        glm::vec2 mousePos = Util::Input::GetCursorPosition();

        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            if (glm::distance(mousePos, m_UpgradeButtonPos) < 40.0f) {
                return -1;
            }

            for (size_t i = 0; i < m_LevelPositions.size() && i < levels.size(); ++i) {
                if (levels[i].unlocked && glm::distance(mousePos, m_LevelPositions[i]) < 50.0f) {
                    return static_cast<int>(i) + 1;
                }
            }
        }
        return 0;
    }

    void Draw(const std::vector<LevelStatus>& levels) {
        Util::Transform bgTransform;
        m_Background->Draw(Util::ConvertToUniformBufferData(
            bgTransform, m_Background->GetSize(), -10.0f));

        // 計算目前影格，若超過 116 就強制卡在 116（回到主頁面時播一次動畫即停住）
        int currentFrame = static_cast<int>(m_AnimationTimer * 24.0f) + 1;
        if (currentFrame > 116) {
            currentFrame = 116;
        }

        for (size_t i = 0; i < m_LevelPositions.size() && i < levels.size(); ++i) {
            if (!levels[i].unlocked) continue;

            Util::Transform t;
            t.translation = m_LevelPositions[i];
            t.scale = {0.8f, 0.8f};

            if (levels[i].stars > 0) {
                // 已通關：從 1 播到 116，之後永久停在 116
                if (m_FlagFrames[currentFrame]) {
                    m_FlagFrames[currentFrame]->Draw(Util::ConvertToUniformBufferData(
                        t, m_FlagFrames[currentFrame]->GetSize(), 0.0f));
                }
                DrawLevelStars(m_LevelPositions[i], levels[i].stars);
            } else {
                // 已解鎖但未通關：靜態第一幀
                if (m_FlagFrames[1]) {
                    m_FlagFrames[1]->Draw(Util::ConvertToUniformBufferData(
                        t, m_FlagFrames[1]->GetSize(), 0.0f));
                }
            }
        }

        Util::Transform upgradeT;
        upgradeT.translation = m_UpgradeButtonPos;
        upgradeT.scale = {1.0f, 1.0f};
        m_UpgradeImage->Draw(Util::ConvertToUniformBufferData(
            upgradeT, m_UpgradeImage->GetSize(), 5.0f));
    }

private:
    // 🎯 修正：調緊星星位置與間距，使其精準、緊湊地貼在旗幟上方
    void DrawLevelStars(glm::vec2 flagPos, int starCount) {
        float starSpacing = 12.0f;
        float yOffset = 20.0f;     // 💡 從 35.0f 降到 15.0f，讓星星往下移動貼近旗幟

        for (int i = 0; i < starCount; ++i) {
            Util::Transform starT;
            float xOffset = (static_cast<float>(i) - static_cast<float>(starCount - 1) / 2.0f) * starSpacing;

            starT.translation = {flagPos.x + xOffset, flagPos.y + yOffset};
            starT.scale = {0.3f, 0.3f};

            m_StarImage->Draw(Util::ConvertToUniformBufferData(
                starT, m_StarImage->GetSize(), 1.0f));
        }
    }

private:
    std::shared_ptr<Util::Image> m_Background;
    std::vector<glm::vec2> m_LevelPositions;
    std::shared_ptr<Util::Image> m_UpgradeImage;
    glm::vec2 m_UpgradeButtonPos;

    std::vector<std::shared_ptr<Util::Image>> m_FlagFrames;
    std::shared_ptr<Util::Image> m_StarImage;
    float m_AnimationTimer = 0.0f;
};

#endif // WORLD_MAP_HPP