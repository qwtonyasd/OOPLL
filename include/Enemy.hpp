#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Unit.hpp"
#include "Util/Image.hpp"

class Enemy : public Unit {
public:
    Enemy(const std::vector<glm::vec2>& path, float speed, float hp, const std::string& imgPath)
        : Unit(path, speed, hp), m_IsBlocked(false), m_ReachedEnd(false) {
        SetDrawable(std::make_shared<Util::Image>(imgPath));
        SetZIndex(10.0f);
    }

    void Update() override {
        if (m_HP <= 0) return;

        if (!m_IsBlocked) {
            if (m_CurrentNodeIdx < m_Path.size()) {
                MoveTowardsNextNode();
            } else {
                m_ReachedEnd = true;
                m_HP = 0; // 到達終點強制死亡
            }
        }
    }

    void OnDeath() {
        // 如果是死於玩家攻擊（非到達終點），則給予金錢
        if (!m_ReachedEnd && m_HP <= 0) {
            // 這裡呼叫你的 GameManager 加錢邏輯，例如：
            // GameManager::GetInstance().AddMoney(10);
        }
    }

    void SetBlocked(bool blocked) { m_IsBlocked = blocked; }
    bool IsBlocked() const { return m_IsBlocked; }
    void TakeDamage(float amount) { m_HP -= amount; }
    float GetHP() const { return m_HP; }

private:
    bool m_IsBlocked;
    bool m_ReachedEnd;
};

#endif