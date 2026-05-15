#ifndef BOMB_TOWER_H
#define BOMB_TOWER_H

#include "Tower.hpp"
#include "Tower/Projectile/Projectile.hpp"
#include <vector>
#include <string>
#include <memory>
#include <glm/glm.hpp>

class BombTower : public Tower {
public:
    // 繼承自 Tower 的自定義數據結構，用來儲存炸彈塔每一級的特殊資源路徑
    struct BombStats : public TowerStats {
        std::string towerAnimFolder;   // 塔本體攻擊動畫的資料夾路徑
        std::string explosionFolder;   // 爆炸特效動畫的資料夾路徑 (1.png ~ 21.png)
        std::string bulletSprite;      // 飛行中子彈的圖片路徑
        Projectile::MoveType moveType; // 新增：移動類型 (拋物線或追蹤旋轉)
    };

    BombTower(glm::vec2 pos);
    virtual ~BombTower(); // <--- 補上這一行宣告
    void Draw() override;
    void Attack(std::shared_ptr<Enemy> target,
                std::vector<std::shared_ptr<Enemy>>& allEnemies,
                std::vector<std::shared_ptr<Projectile>>& projectiles) override;
    void UpdateAnimation() override;
    void Upgrade() override;

private:
    /**
     * @brief 根據當前等級 (m_Level) 重新載入對應的動畫幀素材
     */
    void LoadLevelAssets();

    // --- 數據與資源管理 ---
    // 儲存高等級的數值與資源路徑清單
    std::vector<BombStats> m_BombStats;

    // --- 動態狀態管理 ---
    bool m_IsAttacking = false;           // 是否正在播放攻擊動畫
    bool m_HasFired = false;              // 在一次動畫中是否已經產生過子彈
    float m_AttackStartTime = 0.0f;       // 攻擊開始的時間戳記
    std::vector<std::string> m_AttackFrames; // 存儲當前等級的所有動畫幀路徑

    // --- 暫存指標 (用於在 UpdateAnimation 中產生子彈) ---
    std::shared_ptr<Enemy> m_CurrentTarget;
    std::vector<std::shared_ptr<Enemy>>* m_AllEnemiesRef = nullptr;
    std::vector<std::shared_ptr<Projectile>>* m_ProjectilesRef = nullptr;
};

#endif // BOMB_TOWER_H