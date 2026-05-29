#pragma once
#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"
#include "Util/Image.hpp"
#include "Enemy.hpp"
#include <memory>
#include <vector>
#include <string>

class FireballProjectile : public Util::GameObject {
private:
    glm::vec2 m_StartPos;
    glm::vec2 m_TargetPos;
    float m_Progress = 0.0f;
    float m_Speed = 2.0f; // 飛行速度
    float m_Damage;
    float m_Radius;

    bool m_IsExploding = false;
    float m_ExplosionTimer = 0.0f;
    const float m_ExplosionDuration = 0.9f; // 爆炸動畫總時長

public:
    FireballProjectile(glm::vec2 start, glm::vec2 target, float dmg, float radius)
        : m_StartPos(start), m_TargetPos(target), m_Damage(dmg), m_Radius(radius) {

        // 初始狀態：設定火球飛行圖片
        SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/Skill/FireBall/Animation/1.png"));
        m_Transform.translation = start;
        m_ZIndex = 40.0f;
    }

    // 移除了 override，因為父類別 GameObject 沒有虛擬的 Update
    bool Update(std::vector<std::shared_ptr<Enemy>>& enemies, float dt) {
        if (!m_IsExploding) {
            // --- 飛行階段 ---
            m_Progress += dt * m_Speed;

            // 線性插值計算位置 (不管 Y 是正還是負，這行代碼都能正確從 start 飛到 target)
            m_Transform.translation = m_StartPos + (m_TargetPos - m_StartPos) * m_Progress;

            // 視覺特效：由小變大
            float scale = 0.5f + (m_Progress * 1.0f);
            m_Transform.scale = {scale, scale};

            // 修正旋轉：如果你希望火球「頭朝下」下落，固定設為 90 度即可
            // 避免使用 += dt，否則它會瘋狂旋轉
            m_Transform.rotation = 90.0f;

            if (m_Progress >= 1.0f) {
                TriggerExplosion(enemies);
            }
        } else {
            // --- 爆炸階段 ---
            m_ExplosionTimer += dt;
            if (m_ExplosionTimer >= m_ExplosionDuration) {
                return false;
            }
        }
        return true;
    }

private:
    void TriggerExplosion(std::vector<std::shared_ptr<Enemy>>& enemies) {
        if (m_IsExploding) return; // 防止重複觸發
        m_IsExploding = true;
        m_Transform.rotation = 0.0f;
        // 爆炸時將大小恢復為 1.0 或根據你的需求調整
        m_Transform.scale = {1.2f, 1.2f};
        // 1. 建立爆炸動畫的路徑列表
        std::vector<std::string> animPaths;
        for (int i = 7; i <= 24; ++i) {
            animPaths.push_back("../PTSD/assets/sprites/images/Skill/FireBall/Animation/" + std::to_string(i) + ".png");
        }

        // 2. 建立並啟動動畫
        // 參數說明:
        // paths: 圖片路徑陣列
        // play: true (立刻開始播放)
        // interval: 80 (每幀 80ms，讓動畫看起來更順暢)
        // loop: false (只播一次，播完即消失)
        // reverse: 0 (不反向播放)
        auto explosionAnim = std::make_shared<Util::Animation>(animPaths, true, 80, false, 0);

        // 3. 替換 Drawable 為動畫
        SetDrawable(explosionAnim);

        // 4. 執行傷害邏輯 (範圍傷害)
        glm::vec2 explosionPos = m_Transform.translation;
        for (auto& enemy : enemies) {
            if (!enemy || enemy->GetHP() <= 0) continue;

            // 計算與爆炸中心距離
            float dist = glm::distance(explosionPos, enemy->GetPosition());
            if (dist <= m_Radius) {
                // 給予傷害
                enemy->TakeDamage(m_Damage);
            }
        }
    }
};