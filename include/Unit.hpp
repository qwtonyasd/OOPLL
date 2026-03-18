#ifndef UNIT_HPP
#define UNIT_HPP

#include <vector>
#include <glm/glm.hpp>
#include "Util/GameObject.hpp"
#include "Util/Time.hpp"

class Unit : public Util::GameObject {
public:
    Unit(const std::vector<glm::vec2>& path, float speed, float hp)
        : m_Path(path), m_Speed(speed), m_HP(hp), m_MaxHP(hp) {
        if (!m_Path.empty()) {
            m_Transform.translation = m_Path[0]; // 設定出生點為路徑起點
        }
    }

    virtual ~Unit() = default;

    // 純虛擬函式，讓子類別實作各自的 Update
    virtual void Update() = 0;

    bool IsDead() const { return m_HP <= 0; }

protected:
    /**
     * @brief 核心移動邏輯：所有單位共用
     */
    void MoveTowardsNextNode() {
        if (m_CurrentNodeIdx >= m_Path.size()) return;

        // 將原本已棄用的函式改為 GetDeltaTimeMs()
        float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;

        glm::vec2 currentPos = m_Transform.translation;
        glm::vec2 targetPos = m_Path[m_CurrentNodeIdx];

        glm::vec2 direction = targetPos - currentPos;
        float distance = glm::length(direction);

        if (distance > 1.0f) { // 門檻值，防止抖動
            glm::vec2 velocity = glm::normalize(direction) * m_Speed * dt;

            if (glm::length(velocity) > distance) {
                m_Transform.translation = targetPos;
            } else {
                m_Transform.translation += velocity;
            }
        } else {
            m_CurrentNodeIdx++; // 抵達節點，切換至下一點
        }
    }

    std::vector<glm::vec2> m_Path;
    size_t m_CurrentNodeIdx = 1;
    float m_Speed;
    float m_HP;
    float m_MaxHP;
};

#endif