#ifndef BOMB_HPP
#define BOMB_HPP

#include "Projectile.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"
#include <glm/gtc/constants.hpp>
#include <vector>
#include <string>

class Bomb : public Projectile {
public:
    // 修改建構子：傳入 target 指標（shared_ptr）而不僅是座標
    Bomb(glm::vec2 startPos, std::shared_ptr<Enemy> target, float damage, std::vector<std::shared_ptr<Enemy>>& enemies, Enemy::DamageType damageType)
        : Projectile(startPos, target, damage, damageType), // 傳給基類
          m_StartPos(startPos),
          m_Enemies(enemies) {

        // 初始化落點：如果敵人存在，抓取初始位置；否則設為起點
        if (target) {
            m_FinalLandPos = target->GetPosition();
        } else {
            m_FinalLandPos = startPos;
        }

        std::string basePath = "../PTSD/assets/sprites/images/BombTower/bomb/";
        for (int i = 1; i <= 21; ++i) {
            m_ExplosionFrames.push_back(basePath + std::to_string(i) + ".png");
        }

        SetDrawable(std::make_shared<Util::Image>(m_ExplosionFrames[0]));

        m_IsActive = true;
        m_Transform.translation = startPos;
        m_ZIndex = 50.0f;

        // --- 核心參數调整 ---
        // 飛行時間（秒）。數值越小，炸彈越快落地，敵人越難跑掉。
        m_FlightDuration = 0.8f;
        m_MaxArcHeight = 80.0f;
        m_ElapsedTime = 0.0f;
    }

    void Update() override {
        if (!m_IsActive) return;
        float dt = DeltaTime();

        if (m_IsExploding) {
            m_ExplosionTimer += dt;
            UpdateExplosionAnimation();
        } else {
            m_ElapsedTime += dt;
            float progress = std::clamp(m_ElapsedTime / m_FlightDuration, 0.0f, 1.0f);

            // --- 關鍵修正 1：帶著些微追蹤 ---
            // 如果目標還活著，每一幀都去更新「最終落點」座標。
            // 炸彈會自動修正它的拋物線終點，看起來就像在追蹤敵人。
            if (m_Target && m_Target->GetHP() > 0) {
                m_FinalLandPos = m_Target->GetPosition();
            }

            // 拋物線移動計算（mix 的終點會隨 m_FinalLandPos 變動）
            glm::vec2 currentPos = glm::mix(m_StartPos, m_FinalLandPos, progress);
            float height = m_MaxArcHeight * std::sin(progress * glm::pi<float>());
            m_Transform.translation = currentPos;
            m_Transform.translation.y += height;

            if (progress >= 1.0f) {
                StartExplosion();
            }
        }
    }

private:
    void StartExplosion() {
        m_IsExploding = true;
        m_Transform.rotation = 0.0f;
        // 爆炸發生在炸彈落地的最後座標
        m_Transform.translation = m_FinalLandPos;

        // AOE 範圍傷害
        // --- 關鍵修正 2：範圍傷害決定快跑敵人能否跑掉 ---
        float radius = 50.0f; // 爆炸半徑（像素）

        for (auto& enemy : m_Enemies) {
            if (enemy && enemy->GetHP() > 0) {
                // 判斷敵人是否在爆炸半徑內
                if (glm::distance(m_FinalLandPos, enemy->GetPosition()) <= radius) {
                    enemy->TakeDamage(m_Damage, m_DamageType);
                }
            }
        }
    }

    void UpdateExplosionAnimation() {
        int frameIndex = static_cast<int>(m_ExplosionTimer / 0.05f);

        if (frameIndex < static_cast<int>(m_ExplosionFrames.size())) {
            SetDrawable(std::make_shared<Util::Image>(m_ExplosionFrames[frameIndex]));
        } else {
            m_IsActive = false;
        }
    }

    bool m_IsExploding = false;
    float m_ExplosionTimer = 0.0f;
    std::vector<std::string> m_ExplosionFrames;

    glm::vec2 m_StartPos;
    glm::vec2 m_FinalLandPos; // 這個現在會動態變動
    std::vector<std::shared_ptr<Enemy>>& m_Enemies;
    float m_ElapsedTime;
    float m_FlightDuration;
    float m_MaxArcHeight;
};

#endif