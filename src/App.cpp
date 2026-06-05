#include "App.hpp"
#include "MapFactory.hpp"
#include "EnemyFactory.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Time.hpp"
#include "Util/Logger.hpp"
#include "GameManager.hpp"

void App::Start() {
    LOG_TRACE("App Started");
    m_MapManager = std::make_unique<MapManager>();
    m_WorldMap = std::make_unique<WorldMap>();
    m_TowerManager = std::make_unique<TowerManager>(m_Root);
    m_BuildMenu = std::make_unique<BuildMenu>();
    m_Hud = std::make_unique<Hud>();

    // 初始化法術管理器
    m_SpellManager = std::make_unique<SpellManager>();

    m_VictoryMenu = std::make_unique<VictoryMenu>();
    m_Root.AddChild(m_VictoryMenu->GetGameObject());

    // 🎯 新增：初始化天賦升級選單
    m_UpgradeMenu = std::make_unique<UpgradeMenu>();

    m_IsInGame = false;
    m_CurrentState = State::UPDATE;
}

void App::Update() {
    // 🎯 新增優先判定：如果天賦升級介面開啟中，優先走升級選單的 Update 與 Draw，阻斷遊戲主迴圈
    if (m_UpgradeMenu && m_UpgradeMenu->IsVisible()) {
        float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;
        if (dt > 0.1f) dt = 0.016f;

        m_UpgradeMenu->Update(dt);
        m_UpgradeMenu->Draw();

        // 在升級頁面依然允許按下 ESC 鍵直接關閉整個遊戲
        if (Util::Input::IsKeyDown(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
            m_CurrentState = State::END;
        }
        return; // 直接中斷，避免在背景點選到地圖關卡
    }

    // 原本的遊戲/選關切換邏輯
    if (m_IsInGame) {
        HandleGamePlay();
    } else {
        HandleSelectLevel();
    }

    if (Util::Input::IsKeyDown(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }
}

void App::HandleSelectLevel() {
    int maxUnlocked = 5;
    int selected = m_WorldMap->Update(maxUnlocked);
    m_WorldMap->Draw(maxUnlocked);

    if (selected > 0) {
        LOG_INFO("Entering Level: {}", selected);
        ChangeLevel(selected);
        m_IsInGame = true;
    }
    // 🎯 新增：接收到世界地圖回傳的 -1 特殊代碼，打開升級面板
    else if (selected == -1) {
        LOG_INFO("Opening Upgrade Menu...");
        m_UpgradeMenu->SetVisible(true);
    }
}

void App::HandleGamePlay() {
    auto& gm = GameManager::GetInstance();
    float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;
    if (dt > 0.1f) dt = 0.016f;
    auto currentMap = m_MapManager->GetCurrentMap();

    if (m_VictoryMenu->IsVisible()) {
        m_VictoryMenu->Update();
        if (m_VictoryMenu->IsRestartPressed()) {
            ChangeLevel(m_CurrentLevelID);
            return;
        }
        if (m_VictoryMenu->IsContinuePressed()) {
            m_IsInGame = false;
            m_VictoryMenu->SetVisible(false, 0);
            m_VictoryMenu->ResetFlags();
            return;
        }
    } else {
        // --- 1. 更新邏輯 ---
        m_SpellManager->Update(dt, m_Enemies);

        int currentWaveIdx = gm.GetCurrentWave() - 1;
        if (currentWaveIdx < static_cast<int>(m_Waves.size())) {
            auto& waveConfig = m_Waves[currentWaveIdx];
            if (!m_IsWaveActive) {
                if (m_Enemies.empty()) {
                    m_WaveBreakTimer += dt;
                    if (m_WaveBreakTimer >= 3.0f) {
                        m_IsWaveActive = true;
                        m_WaveTimer = 0.0f;
                        m_WaveBreakTimer = 0.0f;
                        m_PendingSubWaves = waveConfig.subWaves;
                    }
                }
            } else {
                m_WaveTimer += dt;
                for (auto it = m_PendingSubWaves.begin(); it != m_PendingSubWaves.end(); ) {
                    if (m_WaveTimer >= it->spawnDelay) {
                        const auto& route = currentMap->GetRouteByIndex(it->routeIndex);
                        for (auto& enemyType : it->enemies) {
                            m_Enemies.push_back(EnemyFactory::Create(enemyType, route));
                        }
                        it = m_PendingSubWaves.erase(it);
                    } else {
                        ++it;
                    }
                }
                if (m_PendingSubWaves.empty() && m_Enemies.empty()) {
                    m_IsWaveActive = false;
                    if (gm.GetCurrentWave() < static_cast<int>(m_Waves.size())) gm.NextWave();
                    else m_VictoryMenu->SetVisible(true, gm.GetHealth());
                }
            }
        }

        m_TowerManager->UpdateAll(m_Enemies, m_Projectiles);

        // --- 2. 安全更新與清理階段 ---

        // 清理援軍
        m_ActiveReinforcements.erase(
            std::remove_if(m_ActiveReinforcements.begin(), m_ActiveReinforcements.end(),
                [&](auto& soldier) {
                    if (!soldier) return true;
                    soldier->Update(m_Enemies, dt);
                    if (soldier->GetHP() <= 0 && soldier->IsDeadAnimationFinished()) {
                        soldier->ReleaseEnemy();
                        return true;
                    }
                    return false;
                }), m_ActiveReinforcements.end());

        // 清理投射物
        m_Projectiles.erase(
            std::remove_if(m_Projectiles.begin(), m_Projectiles.end(),
                [&](auto& p) { p->Update(); return !p->IsActive(); }),
            m_Projectiles.end());

        // 清理敵人 (核心修正：確保更新時不直接刪除容器)
        m_Enemies.erase(
            std::remove_if(m_Enemies.begin(), m_Enemies.end(),
                [&](auto& enemy) {
                    enemy->Update(m_Enemies, dt);
                    bool shouldRemove = enemy->ReachedEnd() ||
                                       (enemy->GetHP() <= 0 && enemy->IsDeadAnimationFinished());
                    if (shouldRemove) {
                        if (enemy->ReachedEnd()) gm.TakeDamage(0);
                        else gm.AddMoney(enemy->GetType() == Enemy::Type::GOBLIN ? 3 : 9);
                    }
                    return shouldRemove;
                }),
            m_Enemies.end());

        // 🎯 唯一修改之需求部分：等待 remove_if 迴圈安全結束後，再將小蜘蛛統一匯入主戰場
        Egg::ClearSpawnQueue(m_Enemies);

        // --- 3. 輸入處理 ---
        glm::vec2 mousePos = Util::Input::GetCursorPosition();
        if (Util::Input::IsKeyDown(Util::Keycode::NUM_1)) m_SpellManager->SelectFireball();
        if (Util::Input::IsKeyDown(Util::Keycode::NUM_2)) m_SpellManager->SelectReinforce();
        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_RB)) m_SpellManager->CancelSelection();

        if (m_BuildMenu->IsVisible()) {
            Tower::Type selectedType = m_BuildMenu->Update();
            if (selectedType != Tower::Type::NONE) {
                if (m_SelectedSlot) {
                    int cost = Tower::GetBaseCost(selectedType);
                    if (gm.SpendMoney(cost)) {
                        m_TowerManager->AddTower(selectedType, m_SelectedSlot->GetPosition(), {});
                        m_SelectedSlot->SetOccupied(true);
                    }
                }
                m_BuildMenu->SetVisible(false);
                m_SelectedSlot = nullptr;
            } else if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
                if (glm::distance(mousePos, m_BuildMenu->GetTransform().translation) > 100.0f) {
                    m_BuildMenu->SetVisible(false);
                    m_SelectedSlot = nullptr;
                }
            }
        } else if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            SpellManager::SpellType clickedSpell = m_Hud->HandleClick(mousePos);
            if (clickedSpell != SpellManager::SpellType::NONE) {
                if (clickedSpell == SpellManager::SpellType::FIREBALL) m_SpellManager->SelectFireball();
                else if (clickedSpell == SpellManager::SpellType::REINFORCE) m_SpellManager->SelectReinforce();
            } else if (m_SpellManager->GetSelectedSpell() != SpellManager::SpellType::NONE) {
                m_SpellManager->CastCurrentSpell(mousePos, m_Enemies, m_ActiveReinforcements);
            } else {
                bool upgradeHandled = false;
                for (auto& tower : m_TowerManager->GetTowers()) {
                    if (tower->GetIsSelected() && tower->IsUpgradeClicked(mousePos)) {
                        int cost = tower->GetUpgradeCost();
                        if (gm.SpendMoney(cost)) tower->Upgrade();
                        upgradeHandled = true;
                        break;
                    }
                }
                if (!upgradeHandled) {
                    if (!m_TowerManager->HandleClick(mousePos)) {
                        bool hitSlot = false;
                        for (auto& slot : m_TowerSlots) {
                            if (!slot->IsOccupied() && glm::distance(mousePos, slot->GetPosition()) < 50.0f) {
                                m_SelectedSlot = slot;
                                m_BuildMenu->SetPosition(slot->GetPosition());
                                m_BuildMenu->SetVisible(true);
                                m_TowerManager->ClearSelection();
                                hitSlot = true;
                                break;
                            }
                        }
                        if (!hitSlot) m_TowerManager->ClearSelection();
                    }
                }
            }
        }
    }

    // --- 4. 繪圖階段 ---
    m_MapManager->Draw();
    for (auto& slot : m_TowerSlots) slot->Draw();
    m_TowerManager->DrawAll();
    for (auto& soldier : m_ActiveReinforcements) if (soldier) soldier->Draw();
    for (auto& enemy : m_Enemies) enemy->Draw();
    for (auto& proj : m_Projectiles) proj->Draw();
    m_SpellManager->Draw();
    if (m_BuildMenu->IsVisible()) m_BuildMenu->Draw();
    if (m_Hud) m_Hud->Draw(m_SpellManager);
    if (m_VictoryMenu->IsVisible()) m_VictoryMenu->Draw();

    if (Util::Input::IsKeyDown(Util::Keycode::BACKSPACE)) m_IsInGame = false;
}
void App::ChangeLevel(int levelId) {
    m_CurrentLevelID = levelId;
    auto newMap = MapFactory::CreateLevel(levelId);

    m_MapManager->AddLevel(levelId, newMap);
    m_MapManager->SwitchLevel(levelId);

    m_Waves = newMap->GetWaves();

    int startMoney = newMap->GetInitialMoney();
    GameManager::GetInstance().InitLevel(startMoney, 20);
    GameManager::GetInstance().SetTotalWaves(static_cast<int>(m_Waves.size()));

    m_TowerManager->Clear();
    m_Projectiles.clear();
    m_Enemies.clear();
    m_TowerSlots.clear();
    m_PendingSubWaves.clear();

    m_ActiveReinforcements.clear();
    m_SpellManager->CancelSelection();

    for (const auto& pos : newMap->GetTowerSlots()) {
        m_TowerSlots.push_back(std::make_shared<TowerSlot>(pos));
    }

    m_WaveTimer = 0.0f;
    m_WaveBreakTimer = 0.0f;
    m_IsWaveActive = false;

    m_BuildMenu->SetVisible(false);
    if (m_VictoryMenu) m_VictoryMenu->SetVisible(false, 0);
}

void App::End() {
    LOG_TRACE("App Terminated");
}