#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Unit.hpp"
#include "Util/Animation.hpp"
#include <vector>
#include <string>
#include <cmath>
#include <memory>

class Enemy : public Unit {
public:
    enum class Type { GOBLIN, ORC };
    enum class State { MOVE_RIGHT, MOVE_UP, MOVE_DOWN, ATTACK, DEATH };

    Enemy(const std::vector<glm::vec2>& path, float speed, float hp,
          const std::vector<std::vector<std::string>>& moveAnimations,
          const std::vector<std::string>& attackPaths,
          const std::vector<std::string>& deadPaths)
        : Unit(path, speed, hp), m_CurrentState(State::MOVE_RIGHT) {

        // 初始化移動動畫 (需循環)
        m_MoveRightAni = std::make_shared<Util::Animation>(moveAnimations[0], true, 100, true, 0);
        m_MoveUpAni    = std::make_shared<Util::Animation>(moveAnimations[1], true, 100, true, 0);
        m_MoveDownAni  = std::make_shared<Util::Animation>(moveAnimations[2], true, 100, true, 0);

        // 初始化攻擊動畫 (需循環)
        m_AttackAni = std::make_shared<Util::Animation>(attackPaths, true, 100, true, 0);

        // 初始化死亡動畫 (Looping 必須為 false 才能觸發 ENDED 狀態)
        m_DeadAni   = std::make_shared<Util::Animation>(deadPaths, false, 100, false, 0);

        SetDrawable(m_MoveRightAni);
        SetZIndex(10.0f);
    }

    void Update() override {
        // 如果血量歸零且尚未進入死亡狀態
        if (m_HP <= 0 && m_CurrentState != State::DEATH) {
            OnDeath();
            return;
        }

        // 如果已經在播放死亡動畫，跳過移動與攻擊邏輯
        if (m_CurrentState == State::DEATH) return;

        if (!m_IsBlocked) {
            if (m_CurrentNodeIdx < m_Path.size()) {
                glm::vec2 oldPos = m_Transform.translation;
                MoveTowardsNextNode();

                glm::vec2 dir = m_Transform.translation - oldPos;
                if (glm::length(dir) > 0.1f) {
                    UpdateDirection(dir);
                }
            } else {
                m_ReachedEnd = true;
                m_HP = 0;
            }
        } else {
            SetState(State::ATTACK);
        }
    }

    void UpdateDirection(glm::vec2 dir) {
        if (std::abs(dir.x) > std::abs(dir.y)) {
            SetState(State::MOVE_RIGHT);
            m_Transform.scale.x = (dir.x > 0) ? 1.0f : -1.0f;
        } else {
            m_Transform.scale.x = 1.0f;
            if (dir.y > 0) {
                SetState(State::MOVE_UP);
            } else {
                SetState(State::MOVE_DOWN);
            }
        }
    }

    void SetState(State newState) {
        if (m_CurrentState == newState) return;
        m_CurrentState = newState;
        switch (m_CurrentState) {
            case State::MOVE_RIGHT: SetDrawable(m_MoveRightAni); break;
            case State::MOVE_UP:    SetDrawable(m_MoveUpAni);    break;
            case State::MOVE_DOWN:  SetDrawable(m_MoveDownAni);  break;
            case State::ATTACK:     SetDrawable(m_AttackAni);    break;
            case State::DEATH:      SetDrawable(m_DeadAni);      break;
        }
    }

    void OnDeath() {
        if (m_CurrentState == State::DEATH) return;

        SetState(State::DEATH);
        m_IsBlocked = false;

        // 確保死亡動畫從第一幀開始且不循環
        m_DeadAni->SetLooping(false);
        m_DeadAni->SetCurrentFrame(0);
        m_DeadAni->Play();
    }

    bool IsDeadAnimationFinished() const {
        if (m_CurrentState != State::DEATH) return false;

        // 判定方式 A: 框架回傳狀態為 ENDED
        bool isStateEnded = (m_DeadAni->GetState() == Util::Animation::State::ENDED);

        // 判定方式 B: 當前播放幀數已到達最後一幀 (防止 State 沒正確切換)
        bool isLastFrame = (m_DeadAni->GetCurrentFrameIndex() >= m_DeadAni->GetFrameCount() - 1);

        return isStateEnded || isLastFrame;
    }

    glm::vec2 GetPosition() const { return m_Transform.translation; }
    void SetBlocked(bool b) { m_IsBlocked = b; }
    bool IsBlocked() const { return m_IsBlocked; }
    void TakeDamage(float a) { m_HP -= a; }
    float GetHP() const { return m_HP; }
    bool ReachedEnd() const { return m_ReachedEnd; }

private:
    State m_CurrentState;
    bool m_IsBlocked = false;
    bool m_ReachedEnd = false;
    std::shared_ptr<Util::Animation> m_MoveRightAni, m_MoveUpAni, m_MoveDownAni;
    std::shared_ptr<Util::Animation> m_AttackAni, m_DeadAni;
};

#endif