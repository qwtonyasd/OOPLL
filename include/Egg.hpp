#pragma once
#ifndef EGG_HPP
#define EGG_HPP

#include "Enemy.hpp"
#include <vector>
#include <string>
#include <memory>

class Egg : public Enemy {
public:
    Egg(Enemy::Type type, const std::vector<glm::vec2>& path, float speed, float hp,
        const std::vector<std::vector<std::string>>& spiderMoveAnis,
        const std::vector<std::string>& spiderAttackPaths,
        const std::vector<std::string>& spiderDeadPaths,
        const std::vector<std::string>& eggHatchPaths,
        const glm::vec2& spawnPos)
        : Enemy(type, path, 0.0f, hp, { eggHatchPaths, eggHatchPaths, eggHatchPaths }, spiderAttackPaths, spiderDeadPaths),
          m_HatchTimer(0.0f),
          m_SpawnPos(spawnPos) {

        (void)speed;
        (void)spiderMoveAnis;

        m_Transform.translation = spawnPos;
        m_HatchDuration = (eggHatchPaths.size() * 0.045f) * 2.0f;
        if (m_HatchDuration < 2.0f) m_HatchDuration = 4.0f;
    }

    static std::vector<std::shared_ptr<Enemy>> s_SpawnQueue;

    static void ClearSpawnQueue(std::vector<std::shared_ptr<Enemy>>& mainEnemyList) {
        if (!s_SpawnQueue.empty()) {
            for (auto& spider : s_SpawnQueue) {
                mainEnemyList.push_back(spider);
            }
            s_SpawnQueue.clear();
        }
    }

    void Update(std::vector<std::shared_ptr<Enemy>>& enemies, float dt) override {
        UpdatePoison(dt);

        // 如果被打死，走原本 Enemy 的死亡更新
        if (m_HP <= 0 || m_CurrentState == State::DEATH) {
            Enemy::Update(enemies, dt);
            return;
        }

        // 🎯 修正 1：刪除 m_MoveRightAni->Update(); 避免 private 存取錯誤
        // PTSD 框架會自動透過場景樹更新動畫，不需手動呼叫。

        m_Transform.translation = m_SpawnPos;
        m_HatchTimer += dt;

        if (m_HatchTimer >= m_HatchDuration) {
            Hatch(enemies);
            m_HP = 0.0f;
            SetState(State::DEATH);
        }
    }

    // 🎯 修正 2：移除 override 關鍵字（因為基底類別沒有這個虛擬函式）
    // 另外提供外接介面供 App.cpp 檢查這個蛋是否該被清掉
    bool IsEggReadyToRemove() {
        // 如果是孵化時間到了，或者血量歸零且原本的死亡判定過了
        if (m_HatchTimer >= m_HatchDuration) {
            return true;
        }
        // 如果是被打死的，因為沒有基底虛擬函式，直接看血量歸零即可
        return m_HP <= 0;
    }

    void Draw() override {
        if (m_CurrentState == State::DEATH && m_HatchTimer >= m_HatchDuration) return;
        Enemy::Draw();
    }

private:
    float m_HatchTimer;
    float m_HatchDuration;
    glm::vec2 m_SpawnPos;

    void Hatch(std::vector<std::shared_ptr<Enemy>>& enemies);
};

#endif // EGG_HPP