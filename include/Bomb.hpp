#ifndef BOMB_HPP
#define BOMB_HPP

#include "Projectile.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"
#include "Util/Logger.hpp"
#include <glm/gtc/constants.hpp>
#include <vector>
#include <algorithm>

class Bomb : public Projectile {
public:
    Bomb(glm::vec2 startPos, glm::vec2 targetPos, float damage, std::vector<std::shared_ptr<Enemy>>& enemies)
        : Projectile(startPos, nullptr, damage),
          m_StartPos(startPos),
          m_FinalLandPos(targetPos),
          m_Enemies(enemies) {

        // 設定圖片路徑
        SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/images/BombTower/bomb/1.png"));

        // 初始化渲染屬性
        m_IsActive = true;
        m_Visible = true;
        m_Transform.scale = {1.0f, 1.0f};
        m_Transform.translation = startPos;
        m_ZIndex = 50.0f;               // 調整為合理的層級，通常 50 就足以蓋過地圖

        // 飛行參數
        m_FlightDuration = 1.2f;
        m_MaxArcHeight = 100.0f;
        m_ElapsedTime = 0.0f;
    }

    void Update() override {
        if (!m_IsActive) return;

        float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;
        m_ElapsedTime += dt;

        float progress = std::clamp(m_ElapsedTime / m_FlightDuration, 0.0f, 1.0f);

        // 線性移動
        glm::vec2 currentPos = glm::mix(m_StartPos, m_FinalLandPos, progress);

        // 拋物線高度
        float height = m_MaxArcHeight * std::sin(progress * glm::pi<float>());

        m_Transform.translation = currentPos;
        m_Transform.translation.y += height;

        if (progress >= 1.0f) {
            Explode();
            m_IsActive = false;
        }
    }

    // 注意：這裡刪除了自定義的 Draw()，使用 Projectile / GameObject 預設的渲染邏輯

private:
    void Explode() {
        float radius = 80.0f;
        for (auto& enemy : m_Enemies) {
            if (enemy && enemy->GetHP() > 0) {
                if (glm::distance(m_FinalLandPos, enemy->GetPosition()) <= radius) {
                    enemy->TakeDamage(m_Damage);
                }
            }
        }
        LOG_DEBUG("Bomb Exploded at ({}, {})", m_FinalLandPos.x, m_FinalLandPos.y);
    }

    glm::vec2 m_StartPos;
    glm::vec2 m_FinalLandPos;
    std::vector<std::shared_ptr<Enemy>>& m_Enemies;

    float m_ElapsedTime;
    float m_FlightDuration;
    float m_MaxArcHeight;
};

#endif