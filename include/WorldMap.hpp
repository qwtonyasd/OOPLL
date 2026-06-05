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

        // 🎯 核心修改 1：載入 Upgrade 按鈕的 4736 圖示
        m_UpgradeImage = std::make_shared<Util::Image>("../PTSD/assets/sprites/images/Upgrade/4736.png");

        m_LevelPositions = {
            {-240, -170}, {-120, 40}, {140, -80}, {170, 110}, {160, 170}
        };

        // 🎯 核心修改 2：設定 Upgrade 按鈕在畫面右下角的座標
        // 這裡設定 (280, -210) 適合標準視窗，如果按鈕跑出螢幕外或太靠近中間，可以微調這組 X, Y 數值
        m_UpgradeButtonPos = {280, -210};
    }

    // 核心邏輯：繪製並檢查點擊
    // 傳回值： 1~5 代表關卡，-1 代表點擊了 Upgrade，0 代表沒點到
    int Update(int maxUnlocked) {
        Draw(maxUnlocked);
        glm::vec2 mousePos = Util::Input::GetCursorPosition();

        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            // 🎯 核心修改 3：優先檢查玩家是否點擊到右下角的 Upgrade 按鈕
            // 使用 distance 判斷滑鼠與按鈕中心點的距離是否小於 40 像素
            if (glm::distance(mousePos, m_UpgradeButtonPos) < 40.0f) {
                return -1; // 回傳特殊代碼 -1，通知 App 彈出升級選單
            }

            // 檢查點擊關卡旗幟
            for (int i = 0; i < maxUnlocked; ++i) {
                if (glm::distance(mousePos, m_LevelPositions[i]) < 50.0f) {
                    return i + 1; // 回傳關卡編號 (1, 2, 3...)
                }
            }
        }
        return 0; // 沒點到任何東西則回傳 0
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

        // 🎯 核心修改 4：在大地圖右下角畫出 Upgrade 按鈕
        Util::Transform upgradeT;
        upgradeT.translation = m_UpgradeButtonPos;
        upgradeT.scale = {1.0f, 1.0f};

        // Z-Index 給 5.0f，確保按鈕層級高於背景，不會被壓在下面
        m_UpgradeImage->Draw(Util::ConvertToUniformBufferData(
            upgradeT, m_UpgradeImage->GetSize(), 5.0f));
    }

private:
    std::shared_ptr<Util::Image> m_Background;
    std::shared_ptr<Util::Image> m_FlagImage;
    std::vector<glm::vec2> m_LevelPositions;

    // 🎯 核心修改 5：新增 Upgrade 按鈕的資源指標與座標變數
    std::shared_ptr<Util::Image> m_UpgradeImage;
    glm::vec2 m_UpgradeButtonPos;
};

#endif // WORLD_MAP_HPP