#ifndef SHAMAN_HPP
#define SHAMAN_HPP

#include "Enemy.hpp"
#include <iostream>

class Shaman : public Enemy {
public:
    Shaman(Enemy::Type type, const std::vector<glm::vec2>& path, float speed, float hp,
           const std::vector<std::vector<std::string>>& moveAnimations,
           const std::vector<std::string>& attackPaths,
           const std::vector<std::string>& deadPaths,
           const std::vector<std::string>& skillPaths,
           const std::vector<std::string>& effectPaths)
        : Enemy(type, path, speed, hp, moveAnimations, attackPaths, deadPaths) {

        m_SkillAni = std::make_shared<Util::Animation>(skillPaths, false, 150, false, 100);
        m_EffectAni = std::make_shared<Util::Animation>(effectPaths, false, 100, false, 100);
    }

    // --- 關鍵修正：修正 Update 的簽章以匹配 Enemy ---
    void Update(std::vector<std::shared_ptr<Enemy>>& enemies, float dt) override {

        // 1. 如果正在施放技能，處理狀態並阻斷移動邏輯
        if (m_CurrentState == State::SKILL) {
            if (m_SkillAni->GetState() == Util::Animation::State::ENDED) {
                SetState(State::MOVE_RIGHT);
            }
            return;
        }

        // 2. 呼叫父類別的 Update (傳遞正確的 enemies 和 dt)
        Enemy::Update(enemies, dt);

        // 3. 處理技能計時 (使用傳入的 dt 而非固定值 0.016f)
        if (m_CurrentState != State::DEATH) {
            m_SkillTimer += dt;
            if (m_SkillTimer >= m_SkillCooldown) {
                CastSkill();
                m_SkillTimer = 0.0f;
            }
        }
    }

    void Draw() override {
        if (m_CurrentState != State::SKILL) {
            Enemy::Draw();
            return;
        }

        SetDrawable(m_SkillAni);
        Enemy::Draw();

        if (m_EffectAni && m_EffectAni->GetState() != Util::Animation::State::ENDED) {
            float originalZIndex = m_ZIndex;
            SetZIndex(originalZIndex + 0.1f);
            SetDrawable(m_EffectAni);
            GameObject::Draw();
            SetZIndex(originalZIndex);
        }
        SetDrawable(m_SkillAni);
    }

    void SetEnemyList(const std::vector<std::shared_ptr<Enemy>>* enemyList) {
        m_EnemyList = enemyList;
    }

private:
    float m_SkillTimer = 0.0f;
    float m_SkillCooldown = 6.0f;
    float m_HealRange = 120.0f;
    float m_HealAmount = 20.0f;

    std::shared_ptr<Util::Animation> m_SkillAni;
    std::shared_ptr<Util::Animation> m_EffectAni;
    const std::vector<std::shared_ptr<Enemy>>* m_EnemyList = nullptr;

    void CastSkill() {
        SetState(State::SKILL);
        m_SkillAni->SetCurrentFrame(0);
        m_SkillAni->Play();

        if (m_EffectAni) {
            m_EffectAni->SetCurrentFrame(0);
            m_EffectAni->Play();
        }
        HealNearbyEnemies();
    }

    void HealNearbyEnemies() {
        if (m_EnemyList == nullptr) return;

        glm::vec2 myPos = GetPosition();
        for (const auto& enemy : *m_EnemyList) {
            if (enemy->IsDeadAnimationFinished()) continue;

            if (glm::distance(myPos, enemy->GetPosition()) <= m_HealRange) {
                enemy->Heal(m_HealAmount);
            }
        }
    }
};

#endif