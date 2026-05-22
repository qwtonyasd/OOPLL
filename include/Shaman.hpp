#ifndef SHAMAN_HPP
#define SHAMAN_HPP

#include "Enemy.hpp"
#include <iostream>

class Shaman : public Enemy {
public:
    // 建構子：接收所有 Enemy 參數，額外傳入本體施法動作與技能特效路徑
    Shaman(Enemy::Type type, const std::vector<glm::vec2>& path, float speed, float hp,
           const std::vector<std::vector<std::string>>& moveAnimations,
           const std::vector<std::string>& attackPaths,
           const std::vector<std::string>& deadPaths,
           const std::vector<std::string>& skillPaths,
           const std::vector<std::string>& effectPaths)
        : Enemy(type, path, speed, hp, moveAnimations, attackPaths, deadPaths) {

        // 初始化本體施法動畫：不循環
        m_SkillAni = std::make_shared<Util::Animation>(skillPaths, false, 150, false, 100);

        // 初始化綠色十字特效動畫：不循環
        m_EffectAni = std::make_shared<Util::Animation>(effectPaths, false, 100, false, 100);
    }

    void Update() override {
        // 1. 如果正在施放技能，檢查技能動畫是否播放完畢
        if (m_CurrentState == State::SKILL) {
            // 由於 m_EffectAni 沒有作為獨立的 GameObject 放入更新清單，
            // 且 PTSD 將 Animation::Update 設為私有，它通常會隨著被設為 Drawable 時在 Draw 或內部自動更新。
            // 這裡我們主要追蹤本體施法動畫 m_SkillAni 的狀態。

            // 當本體施法動畫播放完畢（到最後一幀）時
            if (m_SkillAni->GetState() == Util::Animation::State::ENDED) {
                SetState(State::MOVE_RIGHT); // 切換回移動狀態，底層會自動換回走路的 Drawable
            }
            return; // 阻斷原本 Enemy 的 Update 移動邏輯
        }

        // 2. 執行原本 Enemy 的基本邏輯（移動、攻擊、受傷、死亡處理）
        Enemy::Update();

        // 如果怪物已經死亡，就不需要處理技能計時
        if (m_CurrentState == State::DEATH) return;

        // 3. 技能冷卻計時器
        m_SkillTimer += 0.016f; // 假設固定 DeltaTime 0.016s
        if (m_SkillTimer >= m_SkillCooldown) {
            CastSkill();
            m_SkillTimer = 0.0f;
        }
    }

    // 核心修正：覆寫不帶參數的 Draw()，這與 GameObject::Draw 完美對齊
    void Draw() override {
        // 1. 如果不是處於施放技能狀態，直接走原本 Enemy 的繪製邏輯（畫走路/攻擊/死亡動畫 + 血條）
        if (m_CurrentState != State::SKILL) {
            Enemy::Draw();
            return;
        }

        // 2. 如果正在放技能，我們需要「雙重疊加繪製」：

        // A. 先繪製薩滿的本體施法動畫
        SetDrawable(m_SkillAni);
        Enemy::Draw(); // 這會畫出本體施法動作與血條

        // B. 接著利用同一個 GameObject 容器，換上綠色十字特效動畫再畫一次
        if (m_EffectAni && m_EffectAni->GetState() != Util::Animation::State::ENDED) {
            // 稍微調高 ZIndex 確保特效蓋在本體上面
            float originalZIndex = m_ZIndex;
            SetZIndex(originalZIndex + 0.1f);

            SetDrawable(m_EffectAni);
            GameObject::Draw(); // 呼底層的 Draw 依據當前座標把特效畫出來

            // 畫完後把 ZIndex 還原
            SetZIndex(originalZIndex);
        }

        // C. 還原主要 Drawable，避免影響到其他邏輯判定
        SetDrawable(m_SkillAni);
    }

    /**
     * 設定當前關卡的怪物列表指標，讓薩滿能定位周圍的隊友
     * @param enemyList 傳入包含所有怪物的 vector 指標
     */
    void SetEnemyList(const std::vector<std::shared_ptr<Enemy>>* enemyList) {
        m_EnemyList = enemyList;
    }

private:
    float m_SkillTimer = 0.0f;
    float m_SkillCooldown = 6.0f;     // 每 6 秒放一次補血技能
    float m_HealRange = 120.0f;       // 補血的範圍半徑
    float m_HealAmount = 20.0f;       // 每次恢復的血量

    std::shared_ptr<Util::Animation> m_SkillAni;   // 薩滿本體施法動作
    std::shared_ptr<Util::Animation> m_EffectAni;  // 補血十字特效
    const std::vector<std::shared_ptr<Enemy>>* m_EnemyList = nullptr; // 指向關卡怪物列表

    // 觸發施放技能
    void CastSkill() {
        SetState(State::SKILL);

        // 1. 本體動作重設並播放
        m_SkillAni->SetCurrentFrame(0);
        m_SkillAni->Play();

        // 2. 綠色特效重設並播放
        if (m_EffectAni) {
            m_EffectAni->SetCurrentFrame(0);
            m_EffectAni->Play();
        }

        // 觸發範圍補血
        HealNearbyEnemies();
    }

    // 掃描周圍隊友並進行治療
    void HealNearbyEnemies() {
        if (m_EnemyList == nullptr) {
            std::cout << "[Warning] Shaman 未設定怪物清單 (EnemyList)，無法執行回血！" << std::endl;
            return;
        }

        glm::vec2 myPos = GetPosition();

        for (const auto& enemy : *m_EnemyList) {
            if (enemy->IsDeadAnimationFinished()) continue;

            glm::vec2 targetPos = enemy->GetPosition();
            float distance = glm::distance(myPos, targetPos);

            if (distance <= m_HealRange) {
                enemy->Heal(m_HealAmount);
                std::cout << "[Skill] Shaman 治癒了範圍內的隊友，回復了 " << m_HealAmount << " 點 HP！" << std::endl;
            }
        }
    }
};

#endif // SHAMAN_HPP