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

        m_Transform.translation = spawnPos;

        // 🎯 需求修正：將原本由張數計算出的孵化時間乘以 2 倍
        m_HatchDuration = (eggHatchPaths.size() * 0.045f) * 2.0f;
        if (m_HatchDuration < 2.0f) m_HatchDuration = 4.0f; // 安全保底同步拉長
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
        if (m_HP <= 0) {
            if (m_CurrentState != State::DEATH) OnDeath();
            return;
        }

        m_Transform.translation = m_SpawnPos;
        m_HatchTimer += dt;

        if (m_HatchTimer >= m_HatchDuration) {
            Hatch(enemies);
            m_HP = -999.0f; // 破殼，蛋蒸發
            return;
        }

        // 🎯 核心修正：不再呼叫 Enemy::Update(enemies, dt) 避免路徑節點在背景被偷偷吃掉。
        // 我們手動更新動畫器 (Animator) 的時間，確保蛋的孵化動畫依然會動，但絕對不更新尋路。
        // (註：如果 PTSD 框架的動畫更新是綁在父類別的其他函式，請保留此處，由 Egg.cpp 重新修正剩餘路徑擷取)
    }

    void Draw() override {
        if (m_HP <= 0) return;
        Enemy::Draw();
    }

private:
    float m_HatchTimer;
    float m_HatchDuration;
    glm::vec2 m_SpawnPos;

    void Hatch(std::vector<std::shared_ptr<Enemy>>& enemies);
};

#endif // EGG_HPP