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
    void SetPoisonEffect(bool enabled) {
        m_HasPoison = enabled;
    }
    void Hit(std::shared_ptr<Enemy> target) {
        target->TakeDamage(m_Damage, m_DamageType);
        if (m_HasPoison) {
            target->ApplyPoison(5.0f, 4.0f); // 毒 2 滴/秒，持續 4 秒
        }
    }

    enum class MoveType { PARABOLA_ROTATE, PARABOLA_SIMPLE, STRAIGHT };
    enum class HitType { SINGLE, AREA };

    Projectile(MoveType move, HitType hit, glm::vec2 start, std::shared_ptr<Enemy> target,
               float damage, float duration, float arcHeight,
               const std::string& spritePath, const std::vector<std::string>& hitFrames,
               std::vector<std::shared_ptr<Enemy>>* allEnemies = nullptr)
        : m_MoveType(move), m_HitType(hit), m_StartPos(start), m_Target(target),
          m_AllEnemies(allEnemies), m_Damage(damage), m_FlightDuration(duration),
          m_MaxArcHeight(arcHeight) {

        m_Transform.translation = start;
        m_ZIndex = 100.0f;
        SetDrawable(std::make_shared<Util::Image>(spritePath));

        m_FinalLandPos = (m_Target) ? m_Target->GetPosition() : start;

        for (const auto& path : hitFrames) {
            m_HitImages.push_back(std::make_shared<Util::Image>(path));
        }
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

    Enemy::DamageType m_DamageType; // 補上這行
    bool m_HasPoison = false;       // 補上這行，預設不中毒

    void UpdateFlight(float dt) {
        m_ElapsedTime += dt;
        float progress = std::clamp(m_ElapsedTime / m_FlightDuration, 0.0f, 1.0f);

        if (m_Target && m_Target->GetHP() > 0) {
            m_FinalLandPos = m_Target->GetPosition();
        }

        glm::vec2 oldPos = m_Transform.translation;
        glm::vec2 currentGroundPos = glm::mix(m_StartPos, m_FinalLandPos, progress);

        float height = (m_MoveType != MoveType::STRAIGHT) ?
                        m_MaxArcHeight * std::sin(progress * glm::pi<float>()) : 0.0f;

        m_Transform.translation = currentGroundPos + glm::vec2(0, height);

        if (m_MoveType != MoveType::PARABOLA_SIMPLE) {
            glm::vec2 dir = m_Transform.translation - oldPos;
            if (glm::length(dir) > 0.0001f) {
                m_Transform.rotation = std::atan2(dir.y, dir.x);
            }
        }

        if (progress >= 1.0f) {
            HandleImpact();
        }
    }

    void HandleImpact() {
        m_IsHitting = true;
        m_Transform.rotation = 0.0f;

        if (m_HitType == HitType::AREA && m_AllEnemies) {
            for (auto& enemy : *m_AllEnemies) {
                if (enemy && enemy->GetHP() > 0 && glm::distance(m_Transform.translation, enemy->GetPosition()) < 50.0f) {
                    // [修改這裡]：改為呼叫 Hit 函式，它會處理傷害與中毒
                    Hit(enemy);
                }
            }
        } else if (m_Target && m_Target->GetHP() > 0) {
            // [修改這裡]：改為呼叫 Hit 函式
            Hit(m_Target);
        }
    }

    void UpdateHitAnimation(float dt) {
        m_HitTimer += dt;
        // 計算當前幀數
        size_t frame = static_cast<size_t>(m_HitTimer / 0.05f);

        // [關鍵修正]：在存取 vector 之前檢查邊界
        if (!m_HitImages.empty()) {
            if (frame < m_HitImages.size()) {
                SetDrawable(m_HitImages[frame]);
            } else {
                // 如果幀數已超過最後一幀，代表動畫結束
                m_IsActive = false;
            }
        } else {
            // 如果沒有爆炸動畫素材，擊中後直接消滅
            m_IsActive = false;
        }
    }

    MoveType m_MoveType;
    HitType m_HitType;
    glm::vec2 m_StartPos;
    glm::vec2 m_FinalLandPos;
    std::shared_ptr<Enemy> m_Target;
    std::vector<std::shared_ptr<Enemy>>* m_AllEnemies;
    float m_Damage, m_ElapsedTime = 0.0f, m_FlightDuration, m_MaxArcHeight;
    bool m_IsActive = true, m_IsHitting = false;
    float m_HitTimer = 0.0f;
    std::vector<std::shared_ptr<Util::Image>> m_HitImages;
};
#endif