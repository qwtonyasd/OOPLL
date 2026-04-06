#ifndef GAMEMANAGER_HPP
#define GAMEMANAGER_HPP

#include <iostream>
#include <string>

class GameManager {
public:
    // 取得單例（方便在任何地方存取金錢）
    static GameManager& GetInstance() {
        static GameManager instance;
        return instance;
    }

    // 初始狀態：第一關 265 塊, 20 點生命
    void InitLevel(int startMoney = 265, int startHealth = 20) {
        m_Money = startMoney;
        m_Health = startHealth;
    }

    // 金錢邏輯
    int GetMoney() const { return m_Money; }
    bool SpendMoney(int amount) {
        if (m_Money >= amount) {
            m_Money -= amount;
            return true;
        }
        return false; // 錢不夠
    }
    void AddMoney(int amount) { m_Money += amount; }

    // 生命邏輯
    int GetHealth() const { return m_Health; }
    void TakeDamage(int amount) {
        m_Health -= amount;
        if (m_Health <= 0) {
            m_Health = 0;
            TriggerGameOver();
        }
    }

private:
    GameManager() = default;
    int m_Money = 0;
    int m_Health = 0;

    void TriggerGameOver() {
        std::cout << "Game Over!" << std::endl;
        // 這裡可以切換遊戲狀態到結束畫面
    }
};

#endif