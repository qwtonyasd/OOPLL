#ifndef GAMEMANAGER_HPP
#define GAMEMANAGER_HPP

#include <iostream>
#include <string>

class GameManager {
public:
    static GameManager& GetInstance() {
        static GameManager instance;
        return instance;
    }

    void InitLevel(int startMoney = 265, int startHealth = 20) {
        m_Money = startMoney;
        m_Health = startHealth;
        m_CurrentWave = 1;
        // 這裡不需要硬編碼 m_TotalWaves = 7，因為 App.cpp 會透過 SetTotalWaves 設定
    }

    // 金錢邏輯
    int GetMoney() const { return m_Money; }
    bool SpendMoney(int amount) {
        if (m_Money >= amount) {
            m_Money -= amount;
            return true;
        }
        return false;
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

    // 波次邏輯
    int GetCurrentWave() const { return m_CurrentWave; }
    int GetTotalWaves() const { return m_TotalWaves; }

    // 補上這個函式來修復編譯錯誤
    void SetTotalWaves(int waves) { m_TotalWaves = waves; }

    void NextWave() {
        if (m_CurrentWave < m_TotalWaves) {
            m_CurrentWave++;
        }
    }

private:
    GameManager() = default;
    int m_Money = 0;
    int m_Health = 0;
    int m_CurrentWave = 1;
    int m_TotalWaves = 0; // 預設改為 0，由關卡初始化時設定

    void TriggerGameOver() {
        std::cout << "Game Over!" << std::endl;
    }
};

#endif