#ifndef HUD_HPP
#define HUD_HPP

#include "Util/Image.hpp"
#include "Util/Text.hpp"
#include "Util/TransformUtils.hpp"
#include "GameManager.hpp"
#include "Util/Color.hpp"
#include "MageSkill\SpellManager.hpp" // 必須引入以識別 SpellType 與 SpellManager 指針
#include <memory>
#include <string>
#include <vector>

class Hud {
public:
    // 新增法術按鈕結構體，方便統一管理
    struct SpellButton {
        SpellManager::SpellType type;
        glm::vec2 position;
        glm::vec2 size;
        std::shared_ptr<Util::Image> icon;
        std::shared_ptr<Util::Image> mask;
    };

    Hud();        // 建構子宣告

    // 修改：傳入 SpellManager 的指標，讓 Hud 能即時抓取冷卻時間與選取狀態
    void Draw(const std::unique_ptr<SpellManager>& spellManager);

    // 新增：提供給 App.cpp 呼叫的滑鼠點擊 UI 判定
    SpellManager::SpellType HandleClick(const glm::vec2& mousePos);

private:
    // 輔助繪製文字的功能
    void DrawValue(const std::string& content, glm::vec2 pos, Util::Color color, int fontSize);

    std::shared_ptr<Util::Image> m_BgImage;
    std::shared_ptr<Util::Image> m_HeartIcon;
    std::string m_FontPath;

    // 新增：法術按鈕容器
    std::vector<SpellButton> m_SpellButtons;
};

#endif