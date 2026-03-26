#ifndef WORLD_MAP_HPP
#define WORLD_MAP_HPP

#include <vector>
#include <memory>
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Transform.hpp"
#include "Util/Keycode.hpp"
#include "Util/TransformUtils.hpp" // 引入剛才那個檔案
#include <iostream>

class WorldMap {
public:
    WorldMap() {
        m_Background = std::make_shared<Util::Image>("../assets/image_3e4c06.jpg");
        m_FlagImage = std::make_shared<Util::Image>("../assets/flag.png");

        m_LevelPositions = {
            {-340, -160}, {-120, 40}, {150, -90}, {320, 110}, {480, -30}
        };
    }

    int Update(int maxUnlocked) {
        Draw(maxUnlocked);
        glm::vec2 mousePos = Util::Input::GetCursorPosition();

        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            for (int i = 0; i < maxUnlocked; ++i) {
                if (glm::distance(mousePos, m_LevelPositions[i]) < 50.0f) {
                    return i + 1;
                }
            }
        }
        return 0;
    }

private:
    void Draw(int maxUnlocked) {
        // 1. 畫背景
        Util::Transform bgTransform;
        // 使用新發現的函式：ConvertToUniformBufferData(transform, size, zIndex)
        // 背景 ZIndex 設為最低 (例如 -10.0f)
        m_Background->Draw(Util::ConvertToUniformBufferData(
            bgTransform, m_Background->GetSize(), -10.0f));

        // 2. 畫旗幟
        for (int i = 0; i < maxUnlocked; ++i) {
            Util::Transform t;
            t.translation = m_LevelPositions[i];
            t.scale = {0.8f, 0.8f};
            // 旗幟 ZIndex 設高一點 (例如 0.0f)
            m_FlagImage->Draw(Util::ConvertToUniformBufferData(
                t, m_FlagImage->GetSize(), 0.0f));
        }
    }

    std::shared_ptr<Util::Image> m_Background;
    std::shared_ptr<Util::Image> m_FlagImage;
    std::vector<glm::vec2> m_LevelPositions;
};

#endif