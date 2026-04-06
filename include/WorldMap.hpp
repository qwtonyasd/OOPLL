#ifndef WORLD_MAP_HPP
#define WORLD_MAP_HPP

#include <vector>
#include <memory>
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Transform.hpp"
#include "Util/TransformUtils.hpp"
#include <iostream>

class WorldMap {
public:
    WorldMap() {
        m_Background = std::make_shared<Util::Image>("../PTSD/assets/sprites/images/start/1.png");
        m_FlagImage = std::make_shared<Util::Image>("../PTSD/assets/sprites/images/Flag/1.png");

        m_LevelPositions = {
            {-240, -170}, {-120, 40}, {150, -90}, {320, 110}, {480, -30}
        };
    }

    // 這裡是核心邏輯：繪製並檢查點擊
    int Update(int maxUnlocked) {
        Draw(maxUnlocked);
        glm::vec2 mousePos = Util::Input::GetCursorPosition();

        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            for (int i = 0; i < maxUnlocked; ++i) {
                // 檢查滑鼠與旗幟座標的距離
                if (glm::distance(mousePos, m_LevelPositions[i]) < 50.0f) {
                    return i + 1; // 回傳關卡編號 (1, 2, 3...)
                }
            }
        }
        return 0; // 沒點到則回傳 0
    }

    void Draw(int maxUnlocked) {
        // 1. 畫背景
        Util::Transform bgTransform;
        m_Background->Draw(Util::ConvertToUniformBufferData(
            bgTransform, m_Background->GetSize(), -10.0f));

        // 2. 畫旗幟
        for (int i = 0; i < maxUnlocked; ++i) {
            Util::Transform t;
            t.translation = m_LevelPositions[i];
            t.scale = {0.8f, 0.8f};
            m_FlagImage->Draw(Util::ConvertToUniformBufferData(
                t, m_FlagImage->GetSize(), 0.0f));
        }
    }

private:
    std::shared_ptr<Util::Image> m_Background;
    std::shared_ptr<Util::Image> m_FlagImage;
    std::vector<glm::vec2> m_LevelPositions;
};

#endif