#ifndef BARRACKS_HPP
#define BARRACKS_HPP

#include "Tower.hpp"
#include "Soldier.hpp"
#include "Util/Logger.hpp"

class Barracks : public Tower {
public:
    Barracks(glm::vec2 pos)
        : Tower(pos, "../PTSD/assets/sprites/images/BarracksTower/TowerLevel1/1.png", 150.0f, 1.0f, 0.0f),
          m_RespawnTimer(0.0f) {
        // 在塔的周圍生成三名士兵
        m_Soldiers.push_back(std::make_shared<Soldier>(pos + glm::vec2{0, -40}));
        m_Soldiers.push_back(std::make_shared<Soldier>(pos + glm::vec2{-30, -60}));
        m_Soldiers.push_back(std::make_shared<Soldier>(pos + glm::vec2{30, -60}));
    }

    void Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) override {
        (void)target; (void)allEnemies; // 兵營不發射子彈
    }

    void Update(std::vector<std::shared_ptr<Enemy>>& enemies) override {
        float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;
        bool anySoldierDead = false;

        // 步驟 1：重置怪物的被阻擋狀態
        for (auto& enemy : enemies) enemy->SetBlocked(false);

        // 步驟 2：為每位活著的小兵尋找目標
        for (auto& s : m_Soldiers) {
            if (s->IsDead()) {
                s->Update(dt, nullptr);
                anySoldierDead = true;
                continue;
            }

            std::shared_ptr<Enemy> myTarget = nullptr;
            glm::vec2 sPos = s->GetTransform().translation;

            // 優先邏輯 A：阻擋（一對一）
            for (auto& enemy : enemies) {
                float dist = glm::distance(sPos, enemy->GetTransform().translation);
                if (dist <= s->GetBlockRange() && !enemy->IsBlocked()) {
                    enemy->SetBlocked(true); // 攔截成功
                    myTarget = enemy;
                    break;
                }
            }

            // 優先邏輯 B：支援（多對一）
            if (!myTarget) {
                for (auto& enemy : enemies) {
                    float dist = glm::distance(sPos, enemy->GetTransform().translation);
                    if (dist <= s->GetSupportRange() && enemy->IsBlocked()) {
                        myTarget = enemy; // 幫隊友打
                        break;
                    }
                }
            }

            // 執行小兵戰鬥邏輯
            s->Update(dt, myTarget);

            // 如果小兵正在跟怪互毆，小兵也要受傷
            if (myTarget) {
                s->TakeDamage(8.0f * dt); // 怪物每秒對兵造成 8 點傷害
            }
        }

        // 步驟 3：處理死亡後的重生計時
        if (anySoldierDead) {
            m_RespawnTimer += dt;
            if (m_RespawnTimer >= 10.0f) {
                for (auto& s : m_Soldiers) if (s->IsDead()) s->Respawn();
                m_RespawnTimer = 0.0f;
            }
        }
    }

    void Draw() override {
        Tower::Draw(); // 畫兵營圖片
        for (auto& s : m_Soldiers) s->Draw(); // 畫小兵圖片
    }

private:
    std::vector<std::shared_ptr<Soldier>> m_Soldiers;
    float m_RespawnTimer;
};

#endif