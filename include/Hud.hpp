#ifndef HUD_HPP
#define HUD_HPP

#include "Util/Image.hpp"
#include "Util/Text.hpp"
#include "Util/TransformUtils.hpp"
#include "GameManager.hpp"
#include "Util/Color.hpp"
#include <memory>
#include <string>

class Hud {
public:
    Hud();        // 建構子宣告
    void Draw();  // 繪製功能宣告

private:
    // 輔助繪製文字的功能
    void DrawValue(const std::string& content, glm::vec2 pos, Util::Color color, int fontSize);

    std::shared_ptr<Util::Image> m_BgImage;
    std::shared_ptr<Util::Image> m_HeartIcon;
    std::string m_FontPath;
};

#endif