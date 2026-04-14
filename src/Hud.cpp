#include "Hud.hpp"

Hud::Hud() {
    std::string baseDir = "../PTSD/assets/sprites/images/Start/";
    m_FontPath = "../PTSD/assets/sprites/images/fonts/337_Calibri.ttf";

    m_BgImage = std::make_shared<Util::Image>(baseDir + "Hud.png");
    m_HeartIcon = std::make_shared<Util::Image>(baseDir + "Love.png");
}

void Hud::Draw() {
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

    // 波次：根據你要求的，這裡會隨著 gm.GetCurrentWave() 推進
    std::string waveStr = "WAVE " + std::to_string(gm.GetCurrentWave()) + "/" + std::to_string(gm.GetTotalWaves());
    DrawValue(waveStr, {-250, 250}, Util::Color(230, 230, 210, 255), 14);
}

void Hud::DrawValue(const std::string& content, glm::vec2 pos, Util::Color color, int fontSize) {
    auto text = std::make_shared<Util::Text>(m_FontPath, fontSize, content, color);
    Util::Transform t;
    t.translation = pos;
    text->Draw(Util::ConvertToUniformBufferData(t, text->GetSize(), 10.2f));
}