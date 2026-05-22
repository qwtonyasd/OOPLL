#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "Util/GameObject.hpp"
#include "Util/Time.hpp"
#include "Util/Image.hpp"
#include "Enemy.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

class Projectile : public Util::GameObject {
public:
    enum class MoveType {
        PARABOLA_ROTATE, // A類：拋物線 + 旋轉 (箭矢)
        PARABOLA_SIMPLE, // B類: 純拋物線 (炸彈)
        STRAIGHT         // C類: 直線 (魔法彈)
    };

    enum class HitType {
        SINGLE,          // E類: 單體
        AREA             // D類: 範圍 (AOE)
    };

    Projectile(MoveType move, HitType hit, glm::vec2 start, std::shared_ptr<Enemy> target,
               float damage, float duration, float arcHeight,
               const std::string& spritePath, const std::vector<std::string>& hitFrames,
               std::vector<std::shared_ptr<Enemy>>* allEnemies = nullptr)
        : m_MoveType(move), m_HitType(hit), m_StartPos(start), m_Target(target),
          m_AllEnemies(allEnemies), m_Damage(damage), m_FlightDuration(duration),
          m_MaxArcHeight(arcHeight), m_HitFrames(hitFrames) { // 按照宣告順序初始化

        m_Transform.translation = start;
        m_ZIndex = 100.0f;
        SetDrawable(std::make_shared<Util::Image>(spritePath));
        if (m_Target) m_FinalLandPos = m_Target->GetPosition();
    }

    void Update() {
        if (!m_IsActive) return;
        float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;

        if (m_IsHitting) {
            UpdateHitAnimation(dt);
        } else {
            UpdateFlight(dt);
        }
    }

    bool IsActive() const { return m_IsActive; }

private:
    void UpdateFlight(float dt) {
        m_ElapsedTime += dt;
        float progress = std::clamp(m_ElapsedTime / m_FlightDuration, 0.0f, 1.0f);

        // 如果目標還活著，更新最終落點（追蹤效果）
        if (m_Target && m_Target->GetHP() > 0) {
            m_FinalLandPos = m_Target->GetPosition();
        }

        glm::vec2 oldPos = m_Transform.translation; // 紀錄移動前的位置
        glm::vec2 currentGroundPos = glm::mix(m_StartPos, m_FinalLandPos, progress);

        float height = 0.0f;
        if (m_MoveType != MoveType::STRAIGHT) {
            height = m_MaxArcHeight * std::sin(progress * glm::pi<float>());
        }

        m_Transform.translation = currentGroundPos;
        m_Transform.translation.y += height;

        // --- 核心修正：計算面向角度 ---
        // 無論是箭矢 (PARABOLA_ROTATE) 還是魔法彈 (STRAIGHT)，都計算面向位移方向
        if (m_MoveType == MoveType::PARABOLA_ROTATE || m_MoveType == MoveType::STRAIGHT) {
            glm::vec2 dir = m_Transform.translation - oldPos; // 計算這幀移動的向量
            if (glm::length(dir) > 0.0001f) {
                m_Transform.rotation = std::atan2(dir.y, dir.x);
            }
        }

        if (progress >= 1.0f) HandleImpact();
    }

    void HandleImpact() {
        m_IsHitting = true;
        m_Transform.rotation = 0.0f;

        if (m_HitType == HitType::AREA && m_AllEnemies) {
            for (auto& enemy : *m_AllEnemies) {
                if (enemy && glm::distance(m_Transform.translation, enemy->GetPosition()) < 50.0f) {
                    enemy->TakeDamage(m_Damage, Enemy::DamageType::PHYSICAL);
                }
            }
        } else if (m_Target && m_Target->GetHP() > 0) {
            m_Target->TakeDamage(m_Damage, Enemy::DamageType::PHYSICAL);
        }
    }

    void UpdateHitAnimation(float dt) {
        m_HitTimer += dt;
        size_t frame = static_cast<size_t>(m_HitTimer / 0.05f);
        if (frame < m_HitFrames.size()) {
            SetDrawable(std::make_shared<Util::Image>(m_HitFrames[frame]));
        } else {
            m_IsActive = false;
        }
    }

    MoveType m_MoveType; HitType m_HitType;
    glm::vec2 m_StartPos; glm::vec2 m_FinalLandPos;
    std::shared_ptr<Enemy> m_Target;
    std::vector<std::shared_ptr<Enemy>>* m_AllEnemies;
    float m_Damage; float m_ElapsedTime = 0.0f;
    float m_FlightDuration; float m_MaxArcHeight;
    bool m_IsActive = true; bool m_IsHitting = false;
    float m_HitTimer = 0.0f; std::vector<std::string> m_HitFrames;
};
#endif