#ifndef BARRACKS_HPP
#define BARRACKS_HPP

#include "Tower.hpp"
#include "Soldier.hpp"

class Barracks : public Tower {
public:
    Barracks(glm::vec2 pos)
        : Tower(pos, "../PTSD/assets/sprites/images/BarracksTower/TowerLevel1/1.png", 150.0f, 10.0f, 0.0f) {
        // 初始化三隻小兵
        m_Soldiers.push_back(std::make_shared<Soldier>(pos + glm::vec2{0, -40}));
        m_Soldiers.push_back(std::make_shared<Soldier>(pos + glm::vec2{-20, -60}));
        m_Soldiers.push_back(std::make_shared<Soldier>(pos + glm::vec2{20, -60}));
    }

    // 1. 必須實作此函式，才能讓 Barracks 不再是抽象類別
    void Attack(std::shared_ptr<Enemy> target, std::vector<std::shared_ptr<Enemy>>& allEnemies) override {
        (void)target; (void)allEnemies; // 兵營不主動射擊
    }

    // 2. 複寫 Update 處理士兵邏輯
    void Update(std::vector<std::shared_ptr<Enemy>>& enemies) override {
        float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;
        bool hasDead = false;

        for (auto& s : m_Soldiers) {
            s->Update(dt, false); // 目前先設為非戰鬥
            if (s->IsDead()) hasDead = true;
        }

        if (hasDead) {
            m_RespawnTimer += dt;
            if (m_RespawnTimer >= 10.0f) {
                for (auto& s : m_Soldiers) if (s->IsDead()) s->Respawn();
                m_RespawnTimer = 0.0f;
            }
        }
    }

    // 3. 自定義繪製士兵的方法
    void Draw() override {
        GameObject::Draw(); // 畫塔
        for (auto& s : m_Soldiers) s->Draw(); // 畫兵
    }

private:
    std::vector<std::shared_ptr<Soldier>> m_Soldiers;
    float m_RespawnTimer = 0.0f;
};

#endif