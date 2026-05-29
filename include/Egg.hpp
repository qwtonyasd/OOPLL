#ifndef EGG_HPP
#define EGG_HPP

#include "Enemy.hpp"

class Egg : public Enemy {
public:
    enum class EggStage {
        HATCHING,
        SPIDERLING
    };

    Egg(Enemy::Type type, const std::vector<glm::vec2>& path, float speed, float hp,
        const std::vector<std::vector<std::string>>& spiderMoveAnis,
        const std::vector<std::string>& spiderAttackPaths,
        const std::vector<std::string>& spiderDeadPaths,
        const std::vector<std::string>& eggHatchPaths,
        const glm::vec2& spawnPos)
        : Enemy(type, path, speed, hp, spiderMoveAnis, spiderAttackPaths, spiderDeadPaths),
          m_Stage(EggStage::HATCHING) {

        m_Transform.translation = spawnPos;
        m_HatchAni = std::make_shared<Util::Animation>(eggHatchPaths, false, 100, false, 100);
        SetDrawable(m_HatchAni);
        m_HatchAni->Play();
    }

    // 修正點 1：同步補上 (enemies, dt) 參數
    void Update(std::vector<std::shared_ptr<Enemy>>& enemies, float dt) override {
        if (m_Stage == EggStage::HATCHING) {
            // 蛋型態：不尋路移動，只需確保動畫更新（如果 PTSD 的 Animation 需要手動 update 則在此呼叫）
            if (m_HatchAni->GetState() == Util::Animation::State::ENDED) {
                m_Stage = EggStage::SPIDERLING;
                SetState(State::MOVE_RIGHT);
            }
            return;
        }

        // 修正點 2：幼蛛型態，傳入對應參數回歸原本的移動尋路與戰鬥
        Enemy::Update(enemies, dt);
    }

    void Draw() override {
        if (m_Stage == EggStage::HATCHING) {
            GameObject::Draw();
            return;
        }
        Enemy::Draw();
    }

private:
    EggStage m_Stage;
    std::shared_ptr<Util::Animation> m_HatchAni;
};

#endif // EGG_HPP