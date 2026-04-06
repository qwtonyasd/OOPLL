#ifndef HUD_HPP
#define HUD_HPP

#include "Util/Image.hpp"
#include "Util/Text.hpp"
#include "Util/TransformUtils.hpp"
#include "GameManager.hpp"
#include "Util/Color.hpp"

class Hud {
public:
    Hud() {
        std::string baseDir = "../PTSD/assets/sprites/images/Start/";
        // 改用 Arial 或其他「正」的字體，路徑請確認
        m_FontPath = "../PTSD/assets/sprites/images/fonts/337_Calibri.ttf";

        m_BgImage = std::make_shared<Util::Image>(baseDir + "Hud.png");
        m_HeartIcon = std::make_shared<Util::Image>(baseDir + "Love.png");
    }

    void Draw() {
        auto& gm = GameManager::GetInstance();
        float zIndex = 10.0f;

        // 1. HUD 背景框 (保持在左上角)
        Util::Transform bgT;
        bgT.translation = {-265, 265};
        m_BgImage->Draw(Util::ConvertToUniformBufferData(bgT, m_BgImage->GetSize(), zIndex));

        // 2. 愛心圖標 (調整到左上角紅框中心)
        Util::Transform heartT;
        heartT.translation = {-315, 275};
        heartT.scale = {0.6f, 0.6f};
        m_HeartIcon->Draw(Util::ConvertToUniformBufferData(heartT, m_HeartIcon->GetSize(), zIndex + 0.1f));

        // 3. 數值顯示 (使用「正」的座標系統)

        // 血量：放在愛心右邊，垂直置中
        DrawValue(std::to_string(gm.GetHealth()), {-290, 273}, Util::Color(255, 255, 255, 255), 14);

        // 金錢：放在金幣圖標右邊，垂直置中
        DrawValue(std::to_string(gm.GetMoney()), {-225, 273}, Util::Color(255, 225, 250, 255), 14);

        // WAVE：放在下方黑框，避開左側骷髏頭
        DrawValue("WAVE 0/7", {-250, 250}, Util::Color(230, 230, 210, 255), 14);
    }

private:
    void DrawValue(const std::string& content, glm::vec2 pos, Util::Color color, int fontSize) {
        // 每次重新建立 Text 物件以確保字體大小生效 (若框架支援 SetSize 可優化)
        auto text = std::make_shared<Util::Text>(m_FontPath, fontSize, content, color);
        Util::Transform t;
        t.translation = pos;
        text->Draw(Util::ConvertToUniformBufferData(t, text->GetSize(), 10.2f));
    }

    std::shared_ptr<Util::Image> m_BgImage;
    std::shared_ptr<Util::Image> m_HeartIcon;
    std::shared_ptr<Util::Text> m_MainText;
    std::string m_FontPath;
};

#endif