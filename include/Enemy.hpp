#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Unit.hpp"
#include "Util/Animation.hpp"
#include <vector>
#include <string>
#include <memory>
#include <glm/glm.hpp>

// 前向宣告 Soldier 類別
class Soldier;

class Enemy : public Unit {
public:
    // 新增 SHAMAN 到 Type 中
    enum class Type { GOBLIN, ORC, WULF, SHAMAN, OGRE };
    // 新增 SKILL 到 State 中，供薩滿放技能動畫使用
    enum class State { MOVE_RIGHT, MOVE_UP, MOVE_DOWN, ATTACK, SKILL, DEATH };
    enum class DamageType { PHYSICAL, MAGIC };

    Enemy(Enemy::Type type, const std::vector<glm::vec2>& path, float speed, float hp,
          const std::vector<std::vector<std::string>>& moveAnimations,
          const std::vector<std::string>& attackPaths,
          const std::vector<std::string>& deadPaths);

    virtual ~Enemy() = default; // 確保有多型解構子
    virtual void Update() override;
    virtual void Draw() override;

    void TakeDamage(float damage, DamageType damageType = DamageType::PHYSICAL);
    void UpdateDirection(glm::vec2 dir);
    void SetState(State newState);
    void OnDeath();
    bool IsDeadAnimationFinished() const;

    // --- 新增：回血機制 ---
    void Heal(float amount) {
        if (m_CurrentState == State::DEATH) return; // 死了就不能觸發回血
        m_Hp += amount;
        if (m_Hp > m_MaxHp) m_Hp = m_MaxHp; // 防止血量溢出最大值
    }

    // --- 公開存取介面 ---
    bool ReachedEnd() const { return m_ReachedEnd; }
    float GetTotalTravelledDistance() const { return m_TotalDistanceTravelled; }
    Enemy::Type GetType() const { return m_Type; }

    // 修正編譯錯誤：讓 Soldier 可以檢查敵人是否已經被阻擋
    bool IsBlocked() const { return m_IsBlocked; }

    // 取得當前座標
    glm::vec2 GetPosition() const { return m_Transform.translation; }

    /**
     * 設定阻擋狀態
     * @param b 是否被阻擋
     * @param soldier 阻擋該敵人的士兵對象
     */
    void SetBlocked(bool b, std::shared_ptr<Soldier> soldier = nullptr) {
        m_IsBlocked = b;
        m_TargetSoldier = soldier;
    }

protected: // 改為 protected，讓子類別 Shaman 可以存取這些必要的狀態變數
    Enemy::Type m_Type;
    State m_CurrentState;

    bool m_IsBlocked = false;
    bool m_ReachedEnd = false;
    float m_TotalDistanceTravelled = 0.0f;

    std::shared_ptr<Soldier> m_TargetSoldier = nullptr;
    float m_AttackTimer = 0.0f;
    float m_AttackCooldown = 1.0f;

    // 動畫資源
    std::shared_ptr<Util::Animation> m_MoveRightAni, m_MoveUpAni, m_MoveDownAni, m_AttackAni, m_DeadAni;

    // 生命值變數（子類別需要知道目前血量與上限）
    float m_Hp = 0.0f;
    float m_MaxHp = 0.0f; // 記得在 Enemy.cpp 的建構子中初始化：m_Hp = hp; m_MaxHp = hp;
};

#endif // ENEMY_HPP