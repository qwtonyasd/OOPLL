#include "Hud.hpp"

Hud::Hud() {
    std::string baseDir = "../PTSD/assets/sprites/images/Start/";
    m_FontPath = "../PTSD/assets/sprites/images/fonts/337_Calibri.ttf";

    m_BgImage = std::make_shared<Util::Image>(baseDir + "Hud.png");
    m_HeartIcon = std::make_shared<Util::Image>(baseDir + "Love.png");

    // --- 根據你實際的資料夾結構修正路徑 ---
    std::string skillDir = "../PTSD/assets/sprites/images/Skill/";

    // 1. 火球術按鈕 (彩色版用 1.png，冷卻灰色版用 22.png)
    SpellButton fireballBtn;
    fireballBtn.type = SpellManager::SpellType::FIREBALL;
    fireballBtn.position = {-200, -265};
    fireballBtn.size = {60.0f, 60.0f};
    fireballBtn.icon = std::make_shared<Util::Image>(skillDir + "FireBall/1.png");   // 彩色
    fireballBtn.mask = std::make_shared<Util::Image>(skillDir + "FireBall/22.png");  // 灰色
    m_SpellButtons.push_back(fireballBtn);

    // 2. 召喚援軍按鈕 (彩色版用 1.png，冷卻灰色版用 22.png)
    SpellButton reinforceBtn;
    reinforceBtn.type = SpellManager::SpellType::REINFORCE;
    reinforceBtn.position = {-130, -265};
    reinforceBtn.size = {60.0f, 60.0f};
    reinforceBtn.icon = std::make_shared<Util::Image>(skillDir + "Summon/1.png");    // 彩色
    reinforceBtn.mask = std::make_shared<Util::Image>(skillDir + "Summon/22.png");   // 灰色
    m_SpellButtons.push_back(reinforceBtn);
}

// 檢查滑鼠是否點擊在法術按鈕的 AABB 範圍內
SpellManager::SpellType Hud::HandleClick(const glm::vec2& mousePos) {
    for (const auto& btn : m_SpellButtons) {
        if (mousePos.x >= btn.position.x - btn.size.x / 2.0f &&
            mousePos.x <= btn.position.x + btn.size.x / 2.0f &&
            mousePos.y >= btn.position.y - btn.size.y / 2.0f &&
            mousePos.y <= btn.position.y + btn.size.y / 2.0f) {
            return btn.type;
        }
    }
    return SpellManager::SpellType::NONE;
}

void Hud::Draw(const std::unique_ptr<SpellManager>& spellManager) {
    auto& gm = GameManager::GetInstance();
    float zIndex = 10.0f;

    // 1. HUD 背景框
    Util::Transform bgT;
    bgT.translation = {-265, 265};
    m_BgImage->Draw(Util::ConvertToUniformBufferData(bgT, m_BgImage->GetSize(), zIndex));

    // 2. 愛心圖標
    Util::Transform heartT;
    heartT.translation = {-315, 275};
    heartT.scale = {0.6f, 0.6f};
    m_HeartIcon->Draw(Util::ConvertToUniformBufferData(heartT, m_HeartIcon->GetSize(), zIndex + 0.1f));

    // 3. 數值顯示
    // 血量 (連動 gm)
    DrawValue(std::to_string(gm.GetHealth()), {-290, 273}, Util::Color(255, 255, 255, 255), 14);

    // 金錢 (連動 gm)
    DrawValue(std::to_string(gm.GetMoney()), {-225, 273}, Util::Color(255, 225, 250, 255), 14);

    // 波次
    std::string waveStr = "WAVE " + std::to_string(gm.GetCurrentWave()) + "/" + std::to_string(gm.GetTotalWaves());
    DrawValue(waveStr, {-250, 250}, Util::Color(230, 230, 210, 255), 14);


    // ========================================================
    // --- 新增：主動法術按鈕渲染邏輯 (含選取放大與冷卻遮罩) ---
    // ========================================================
    float spellZIndex = 12.0f; // 確保法術 UI 在地圖、防禦塔上方

    for (const auto& btn : m_SpellButtons) {
        Util::Transform btnT;
        btnT.translation = btn.position;

        // 💡 視覺回饋：如果玩家目前選中了這個法術進入瞄準狀態，讓圖示微微變大（1.15倍）
        if (spellManager->GetSelectedSpell() == btn.type) {
            btnT.scale = {1.15f, 1.15f};
        } else {
            btnT.scale = {1.0f, 1.0f};
        }

        // 畫法術亮色底圖
        btn.icon->Draw(Util::ConvertToUniformBufferData(btnT, btn.icon->GetSize(), spellZIndex));

        // 取得當前技能的冷卻比例 (0.0 代表冷卻完畢，1.0 代表剛施放進入完整冷卻)
        float cooldownRatio = 0.0f;
        if (btn.type == SpellManager::SpellType::FIREBALL) {
            cooldownRatio = spellManager->GetFireballCooldownRatio();
        } else if (btn.type == SpellManager::SpellType::REINFORCE) {
            cooldownRatio = spellManager->GetReinforceCooldownRatio();
        }

        // 如果技能還在冷卻中，由下而上壓上一層半透明黑色的圖片作為冷卻進度條
        if (cooldownRatio > 0.0f) {
            Util::Transform maskT;
            // 稍微往下位移，使遮罩縮放時看起來是由下而上退去
            maskT.translation = btn.position + glm::vec2(0.0f, -btn.size.y * (1.0f - cooldownRatio) / 2.0f);
            maskT.scale = {1.0f, cooldownRatio}; // 垂直縮放代表冷卻剩餘時間

            btn.mask->Draw(Util::ConvertToUniformBufferData(maskT, btn.mask->GetSize(), spellZIndex + 0.1f));
        }
    }
}

void Hud::DrawValue(const std::string& content, glm::vec2 pos, Util::Color color, int fontSize) {
    auto text = std::make_shared<Util::Text>(m_FontPath, fontSize, content, color);
    Util::Transform t;
    t.translation = pos;
    text->Draw(Util::ConvertToUniformBufferData(t, text->GetSize(), 10.2f));
}