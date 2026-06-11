#ifndef UPGRADE_MENU_HPP
#define UPGRADE_MENU_HPP

#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Transform.hpp"
#include "Util/TransformUtils.hpp"
#include "GameData.hpp"
class UpgradeMenu {
public:
    UpgradeMenu();
    void Update(float dt);
    void Draw();

    void SetVisible(bool visible) { m_IsVisible = visible; }
    bool IsVisible() const { return m_IsVisible; }

private:
    // 透過死座標陣列查表取得精確位置的輔助函式
    glm::vec2 GetIconPosition(int routeIndex, int levelIndex) const;

    // 輔助函式：用來繪製星數或天賦標籤上的等級數字
    void DrawText(const std::string& text, const glm::vec2& position, float scale);

    bool m_IsVisible = false;

    // --- 基礎介面素材 ---
    std::shared_ptr<Util::Image> m_BgPanel;       // 大木底板 (3378.png)
    std::shared_ptr<Util::Image> m_TitleBgStrip;  // 標題下方的掛軸木板條 (3379.png)
    std::shared_ptr<Util::Image> m_TitleImg;      // UPGRADES 標題文字 (3381.png)
    std::shared_ptr<Util::Image> m_BtnDone;       // DONE 按鈕 (3383.png)
    std::shared_ptr<Util::Image> m_BtnReset;      // RESET 按鈕 (3399.png)

    // --- 總星數狀態列素材 ---
    std::shared_ptr<Util::Image> m_StarBgBar;     // 🆕 黑色斜角底板 (3393.png)
    std::shared_ptr<Util::Image> m_StarIcon;      // 🆕 黃色大星星 (3394.png)

    // --- 天賦圖標右下角小星框素材 ---
    std::shared_ptr<Util::Image> m_LvBgActive;    // 🆕 已升級的星星小灰框 (4163.png)
    std::shared_ptr<Util::Image> m_LvBgInactive;  // 🆕 未升級的灰星小灰框 (4168.png)

    // --- 6條路線 x 5個層級 的完整天賦圖標矩陣 ---
    std::shared_ptr<Util::Image> m_TalentIcons[6][5];

    // --- 🔒 這是你完全改好、完美的死座標微調區（絕對不改動） ---
    const float m_RouteX[6] = { -238.0f, -142.0f, -47.0f, 47.0f, 140.0f, 235.0f };
    const float m_LevelY[5] = { -82.0f, -18.0f, 46.0f, 110.0f, 174.0f };

    const glm::vec2 m_PanelCenter = {0.0f, 0.0f};

    // --- 底部 UI 座標配置區 ---
    glm::vec2 m_DoneBtnPos;
    glm::vec2 m_ResetBtnPos;
    glm::vec2 m_TotalStarPos;     // 🆕 左下角總星數的位置

    // --- 星數與天賦等級系統 ---
    int m_TotalStars = 15;        // 初始可用總星數
    const int m_MaxLevel = 5;
};

#endif // UPGRADE_MENU_HPP