#include "UpgradeMenu.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

UpgradeMenu::UpgradeMenu() {
    // 1. 載入基本面版素材
    m_BgPanel       = std::make_shared<Util::Image>("../PTSD/assets/sprites/images/Upgrade/3378.png");
    m_TitleBgStrip  = std::make_shared<Util::Image>("../PTSD/assets/sprites/images/Upgrade/3379.png");
    m_TitleImg      = std::make_shared<Util::Image>("../PTSD/assets/sprites/images/Upgrade/3381.png");
    m_BtnDone       = std::make_shared<Util::Image>("../PTSD/assets/sprites/images/Upgrade/3383.png");
    m_BtnReset      = std::make_shared<Util::Image>("../PTSD/assets/sprites/images/Upgrade/3399.png");

    // 載入新加入的星數與標籤素材
    m_StarBgBar     = std::make_shared<Util::Image>("../PTSD/assets/sprites/images/Upgrade/3393.png");
    m_StarIcon      = std::make_shared<Util::Image>("../PTSD/assets/sprites/images/Upgrade/3394.png");
    m_LvBgActive    = std::make_shared<Util::Image>("../PTSD/assets/sprites/images/Upgrade/4163.png");
    m_LvBgInactive  = std::make_shared<Util::Image>("../PTSD/assets/sprites/images/Upgrade/4168.png");

    // --- 🎯 底部按鈕與狀態列位置微調區 ---
    m_DoneBtnPos   = { 160.0f, -235.0f };  // DONE 往右挪
    m_ResetBtnPos  = { 40.0f, -235.0f };   // RESET 排在 DONE 的左邊
    m_TotalStarPos = { -200.0f, -235.0f }; // 左下角總星數的中心點位置

    // 2. 自動化載入 6 條路線、由下往上的 30 張天賦圖標
    std::string basePath = "../PTSD/assets/sprites/images/Upgrade/";

    for (int route = 0; route < 6; ++route) {
        int startId = 0;
        switch (route) {
            case 0: startId = 4112; break; // 箭矢欄
            case 2: startId = 4122; break; // 法杖欄
            case 3: startId = 4132; break; // 炸彈欄
            case 4: startId = 4142; break; // 火球欄
            case 5: startId = 4152; break; // 雙劍欄
            default: startId = 0;    break; // 盾牌欄獨立處理跳號
        }

        for (int lv = 0; lv < 5; ++lv) {
            int currentId = 0;

            if (route == 1) {
                if (lv == 0)      currentId = 4104;
                else if (lv == 1) currentId = 4106;
                else if (lv == 2) currentId = 4108;
                else if (lv == 3) currentId = 4104;
                else if (lv == 4) currentId = 4110;
            } else {
                currentId = startId + (lv * 2);
            }

            std::string fullPath = basePath + std::to_string(currentId) + ".png";
            m_TalentIcons[route][lv] = std::make_shared<Util::Image>(fullPath);
        }
    }
}

glm::vec2 UpgradeMenu::GetIconPosition(int routeIndex, int levelIndex) const {
    return m_PanelCenter + glm::vec2(m_RouteX[routeIndex], m_LevelY[levelIndex]);
}

void UpgradeMenu::Update([[maybe_unused]] float dt) {
    if (!m_IsVisible) return;

    glm::vec2 mousePos = Util::Input::GetCursorPosition();

    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
        // 檢查 DONE 按鈕點擊
        if (glm::distance(mousePos, m_DoneBtnPos) < 45.0f) {
            LOG_INFO("Closing Upgrade Menu");
            m_IsVisible = false;
            return;
        }

        // 檢查 RESET 按鈕點擊
        if (glm::distance(mousePos, m_ResetBtnPos) < 45.0f) {
            LOG_INFO("Reset All Talents!");
            for (int i = 0; i < 6; ++i) {
                GameData::GetInstance().totalStars += GameData::GetInstance().talentLevels[i];
                GameData::GetInstance().talentLevels[i] = 0;
            }
            return;
        }

        // 偵測點擊天賦格子
        for (int route = 0; route < 6; ++route) {
            int currentLv = GameData::GetInstance().talentLevels[route];
            if (currentLv < m_MaxLevel) {
                glm::vec2 btnPos = GetIconPosition(route, currentLv);
                if (glm::distance(mousePos, btnPos) < 28.0f) {
                    if (m_TotalStars > 0) {
                        GameData::GetInstance().talentLevels[route]++;
                        m_TotalStars--;
                        LOG_INFO("Route {} Upgraded to Level {}!", route, GameData::GetInstance().talentLevels[route]);
                    } else {
                        LOG_INFO("Not enough stars!");
                    }
                    break;
                }
            }
        }
    }
}

void UpgradeMenu::Draw() {
    if (!m_IsVisible) return;

    // 1. 繪製中央大木面板 (Z-Index: 50.0f)
    Util::Transform bgT;
    bgT.translation = m_PanelCenter;
    m_BgPanel->Draw(Util::ConvertToUniformBufferData(bgT, m_BgPanel->GetSize(), 50.0f));

    // 2. 繪製 3379 掛軸木板條
    Util::Transform stripT;
    stripT.translation = m_PanelCenter + glm::vec2(0.0f, 260.0f);
    m_TitleBgStrip->Draw(Util::ConvertToUniformBufferData(stripT, m_TitleBgStrip->GetSize(), 51.0f));

    // 3. 繪製 UPGRADES 絲帶標題文字
    Util::Transform titleT;
    titleT.translation = m_PanelCenter + glm::vec2(0.0f, 265.0f);
    m_TitleImg->Draw(Util::ConvertToUniformBufferData(titleT, m_TitleImg->GetSize(), 52.0f));

    // 4. 雙層迴圈繪製 6 條路線的所有層級天賦與對應的星星標籤
    for (int route = 0; route < 6; ++route) {
        int activeLv = GameData::GetInstance().talentLevels[route];

        for (int lv = 0; lv < 5; ++lv) {
            Util::Transform iconT;
            glm::vec2 iconPos = GetIconPosition(route, lv);
            iconT.translation = iconPos;
            iconT.scale = {1.0f, 1.0f};

            m_TalentIcons[route][lv]->Draw(Util::ConvertToUniformBufferData(iconT, m_TalentIcons[route][lv]->GetSize(), 53.0f));

            // 🆕 繪製右下角的星數小標籤框 (4163 或 4168)
            Util::Transform starTagT;
            // 將小標籤框定位在圖標右下角 (向右下稍微偏移)
            starTagT.translation = iconPos + glm::vec2(18.0f, -18.0f);
            starTagT.scale = {1.0f, 1.0f};

            if (lv < activeLv) {
                // 已升級解鎖的部分：畫 4163.png（有黃星），並寫上固定對應這格的等級文字 (lv + 1)
                m_LvBgActive->Draw(Util::ConvertToUniformBufferData(starTagT, m_LvBgActive->GetSize(), 54.0f));
                DrawText(std::to_string(lv + 1), iconPos + glm::vec2(25.0f, -20.0f), 0.5f);
            } else {
                // 尚未升級變灰的部分：畫 4168.png（灰星），同樣標記這格原本對應的等級數值
                m_LvBgInactive->Draw(Util::ConvertToUniformBufferData(starTagT, m_LvBgInactive->GetSize(), 54.0f));
                DrawText(std::to_string(lv + 1), iconPos + glm::vec2(25.0f, -20.0f), 0.5f);
            }
        }
    }

    // 5. 繪製底部的 DONE 與 RESET 按鈕
    Util::Transform doneT;
    doneT.translation = m_DoneBtnPos;
    m_BtnDone->Draw(Util::ConvertToUniformBufferData(doneT, m_BtnDone->GetSize(), 52.0f));

    Util::Transform resetT;
    resetT.translation = m_ResetBtnPos;
    m_BtnReset->Draw(Util::ConvertToUniformBufferData(resetT, m_BtnReset->GetSize(), 52.0f));

    // 6. 🆕 組合繪製左下角總星數狀態列 (3393 與 3394)
    // 先畫黑色斜角底板 (3393.png)
    Util::Transform starBarT;
    starBarT.translation = m_TotalStarPos;
    m_StarBgBar->Draw(Util::ConvertToUniformBufferData(starBarT, m_StarBgBar->GetSize(), 51.0f));

    // 再把黃色大星星 (3394.png) 疊在底板的偏左位置
    Util::Transform totalStarT;
    totalStarT.translation = m_TotalStarPos + glm::vec2(-40.0f, 0.0f);
    m_StarIcon->Draw(Util::ConvertToUniformBufferData(totalStarT, m_StarIcon->GetSize(), 52.0f));

    // 在大星星的右邊繪製目前剩餘總星數文字（例如：15）
    DrawText(std::to_string(m_TotalStars), m_TotalStarPos + glm::vec2(10.0f, -5.0f), 1.0f);
}

void UpgradeMenu::DrawText(const std::string& text, const glm::vec2& position, float scale) {
    // 記得在這裡對接到你專案的字型渲染函式（例如 ImGui 或是自訂的 Text GameObject 喔！）
}