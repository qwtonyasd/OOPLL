#include "UpgradeMenu.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include <map>

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
    m_DoneBtnPos   = { 160.0f, -235.0f };
    m_ResetBtnPos  = { 40.0f, -235.0f };
    m_TotalStarPos = { -200.0f, -235.0f };

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

    // 🆕 3. 初始化天賦說明文字庫 (你可以自由修改裡面的字)
    // 路線 0: 箭矢欄
    m_TalentDescriptions[0][0] = "+20 Gold Archer Tower Build Refund";
    m_TalentDescriptions[0][1] = "Level 2: +8% Range";
    m_TalentDescriptions[0][2] = "Level 3: +10% Damage";
    m_TalentDescriptions[0][3] = "Level 4: +5% Range";
    m_TalentDescriptions[0][4] = "Level 5: +10% Damage";

    // 路線 1: 盾牌欄
    m_TalentDescriptions[1][0] = "Level 1: +10% Soldier HP";
    m_TalentDescriptions[1][1] = "Level 2: +10% Damage";
    m_TalentDescriptions[1][2] = "Level 3: +10% Soldier HP";
    m_TalentDescriptions[1][3] = "Level 4: +10% Damage";
    m_TalentDescriptions[1][4] = "Level 5: +5% HP & +5% Damage";

    // 路線 2: 法杖欄
    m_TalentDescriptions[2][0] = "Level 1: +10% Mage Tower Range";
    m_TalentDescriptions[2][1] = "Level 2: +10% Damage";
    m_TalentDescriptions[2][2] = "Level 3: +5% Range";
    m_TalentDescriptions[2][3] = "Level 4: +10% Damage";
    m_TalentDescriptions[2][4] = "Level 5: +5% Range & +5% Damage";

    // 路線 3: 炸彈欄
    m_TalentDescriptions[3][0] = "Level 1: +10% Splash Radius";
    m_TalentDescriptions[3][1] = "Level 2: +10% Range";
    m_TalentDescriptions[3][2] = "Level 3: +10% Damage";
    m_TalentDescriptions[3][3] = "Level 4: +10% Damage";
    m_TalentDescriptions[3][4] = "Level 5: +10% Range";

    // 路線 4: 火球欄
    m_TalentDescriptions[4][0] = "10% Fireball Damage";
    m_TalentDescriptions[4][1] = "-15% Cooldown";
    m_TalentDescriptions[4][2] = "Level 3: +10% Blast Radius";
    m_TalentDescriptions[4][3] = "Level 4: +10% Damage";
    m_TalentDescriptions[4][4] = "Level 5: +10% Blast Radius";

    // 路線 5: 雙劍欄
    m_TalentDescriptions[5][0] = "+10% Damage";
    m_TalentDescriptions[5][1] = "+10% HP";
    m_TalentDescriptions[5][2] = "+10% HP";
    m_TalentDescriptions[5][3] = "+10% HP";
    m_TalentDescriptions[5][4] = "+10% Damage!";
}

glm::vec2 UpgradeMenu::GetIconPosition(int routeIndex, int levelIndex) const {
    return m_PanelCenter + glm::vec2(m_RouteX[routeIndex], m_LevelY[levelIndex]);
}

void UpgradeMenu::Update([[maybe_unused]] float dt) {
    if (!m_IsVisible) return;

    int available = m_TotalEarned - GameData::GetInstance().GetSpentStars();
    glm::vec2 mousePos = Util::Input::GetCursorPosition();

    // 🆕 每一影格重置懸停目標，再重新偵測
    m_HoveredRoute = -1;
    m_HoveredLevel = -1;

    for (int route = 0; route < 6; ++route) {
        for (int lv = 0; lv < 5; ++lv) {
            glm::vec2 iconPos = GetIconPosition(route, lv);
            // 偵測滑鼠跟格子的距離是否小於 28 像素 (也就是滑鼠指在上面)
            if (glm::distance(mousePos, iconPos) < 28.0f) {
                m_HoveredRoute = route;
                m_HoveredLevel = lv;
                break;
            }
        }
        if (m_HoveredRoute != -1) break; // 找到了就跳出
    }

    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
        // 檢查 DONE 按鈕
        if (glm::distance(mousePos, m_DoneBtnPos) < 45.0f) {
            LOG_INFO("Closing Upgrade Menu");
            m_IsVisible = false;
            return;
        }

        // 檢查 RESET 按鈕
        if (glm::distance(mousePos, m_ResetBtnPos) < 45.0f) {
            LOG_INFO("Reset All Talents!");
            for (int i = 0; i < 6; ++i) {
                GameData::GetInstance().talentLevels[i] = 0;
            }
            return;
        }

        // 偵測點擊天賦格子 (改成只允許點擊滑鼠當前正指著的那一格)
        if (m_HoveredRoute != -1 && m_HoveredLevel == GameData::GetInstance().talentLevels[m_HoveredRoute]) {
            int currentLv = GameData::GetInstance().talentLevels[m_HoveredRoute];
            if (currentLv < m_MaxLevel) {
                if (available > 0) {
                    GameData::GetInstance().talentLevels[m_HoveredRoute]++;
                    LOG_INFO("Route {} Upgraded to Level {}!", m_HoveredRoute, GameData::GetInstance().talentLevels[m_HoveredRoute]);
                } else {
                    LOG_INFO("Not enough stars!");
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

            Util::Transform starTagT;
            starTagT.translation = iconPos + glm::vec2(18.0f, -18.0f);
            starTagT.scale = {1.0f, 1.0f};

            if (lv < activeLv) {
                m_LvBgActive->Draw(Util::ConvertToUniformBufferData(starTagT, m_LvBgActive->GetSize(), 54.0f));
                DrawText(std::to_string(+ 1), iconPos + glm::vec2(25.0f, -20.0f), 0.5f);
            } else {
                m_LvBgInactive->Draw(Util::ConvertToUniformBufferData(starTagT, m_LvBgInactive->GetSize(), 54.0f));
                DrawText(std::to_string( + 1), iconPos + glm::vec2(25.0f, -20.0f), 0.5f);
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

    // 6. 繪製左下角總星數狀態列
    Util::Transform starBarT;
    starBarT.translation = m_TotalStarPos;
    m_StarBgBar->Draw(Util::ConvertToUniformBufferData(starBarT, m_StarBgBar->GetSize(), 51.0f));

    Util::Transform totalStarT;
    totalStarT.translation = m_TotalStarPos + glm::vec2(-40.0f, 0.0f);
    m_StarIcon->Draw(Util::ConvertToUniformBufferData(totalStarT, m_StarIcon->GetSize(), 52.0f));

    // 7. 計算最新可用星星並繪製
    int available = m_TotalEarned - GameData::GetInstance().GetSpentStars();

    if (available != m_LastStars) {
        m_LastStars = available;
        m_StarText = std::make_unique<Util::Text>(
            "../PTSD/assets/sprites/images/fonts/7_Comic Book.ttf",
            36,
            std::to_string(available),
            Util::Color(255, 255, 255, 255)
        );
    }

    if (m_StarText && m_StarText->GetSize().x > 0) {
        Util::Transform t;
        t.translation = m_TotalStarPos + glm::vec2(10.0f, -5.0f);
        t.scale = {1.0f, 1.0f};
        m_StarText->Draw(Util::ConvertToUniformBufferData(t, m_StarText->GetSize(), 55.0f));
    }

    // 🆕 8. 【核心追加功能】繪製滑鼠懸停的提示文字 (Tooltip)
    if (m_HoveredRoute != -1 && m_HoveredLevel != -1) {
        glm::vec2 mousePos = Util::Input::GetCursorPosition();

        // 取得該格子的文字敘述
        std::string desc = m_TalentDescriptions[m_HoveredRoute][m_HoveredLevel];

        if (!desc.empty()) {
            // 文字渲染的位置：放在滑鼠右下方一點點，避免遮擋
            glm::vec2 textPos = mousePos + glm::vec2(25.0f, -25.0f);

            // 畫出提示文字（提高 Z-Index 到 90.0f，保證飄在最上層）
            // 這裡刻意調小字型比例 (0.45f) 讓完整敘述可以完美塞進畫面中
            DrawTextAtZ(desc, textPos, 0.45f, 90.0f);
        }
    }
}

void UpgradeMenu::DrawText(const std::string& text, const glm::vec2& position, float scale) {
    DrawTextAtZ(text, position, scale, 55.0f); // 預設走原來的 Z 層級
}

// 🆕 新增支援自訂 Z-Index 的 DrawText 擴充函式，專供 Tooltip 使用
void UpgradeMenu::DrawTextAtZ(const std::string& text, const glm::vec2& position, float scale, float zIndex) {
    if (text.empty()) return;

    static std::map<std::string, std::unique_ptr<Util::Text>> textCache;

    if (textCache.find(text) == textCache.end()) {
        auto tempText = std::make_unique<Util::Text>(
            "../PTSD/assets/sprites/images/fonts/7_Comic Book.ttf",
            36,
            text,
            Util::Color(255, 255, 255, 255)
        );

        if (tempText && tempText->GetSize().x > 0) {
            textCache[text] = std::move(tempText);
        } else {
            return;
        }
    }

    auto& targetText = textCache[text];
    if (targetText && targetText->GetSize().x > 0) {
        Util::Transform t;
        t.translation = position;
        t.scale = {scale, scale};

        targetText->Draw(Util::ConvertToUniformBufferData(t, targetText->GetSize(), zIndex));
    }
}