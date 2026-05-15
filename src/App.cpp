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

    m_VictoryMenu = std::make_unique<VictoryMenu>();
    m_Root.AddChild(m_VictoryMenu->GetGameObject());

    m_IsInGame = false;
    m_CurrentState = State::UPDATE;
}

void App::Update() {
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
    int maxUnlocked = 3;
    int selected = m_WorldMap->Update(maxUnlocked);
    m_WorldMap->Draw(maxUnlocked);

    if (selected > 0) {
        LOG_INFO("Entering Level: {}", selected);
        ChangeLevel(selected);
        m_IsInGame = true;
    }
}

void App::HandleGamePlay() {
    auto& gm = GameManager::GetInstance();
    float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;
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
        // --- 波次邏輯：時間戳記出怪模式 ---
        int currentWaveIdx = gm.GetCurrentWave() - 1;
        if (currentWaveIdx < static_cast<int>(m_Waves.size())) {
            auto& waveConfig = m_Waves[currentWaveIdx];

            if (!m_IsWaveActive) {
                if (m_Enemies.empty()) {
                    m_WaveBreakTimer += dt;
                    if (m_WaveBreakTimer >= 3.0f) {
                        m_IsWaveActive = true;
                        m_WaveTimer = 0.0f;      // 重置時間
                        m_WaveBreakTimer = 0.0f;
                        m_PendingSubWaves = waveConfig.subWaves; // 複製配置到暫存
                    }
                }
            } else {
                m_WaveTimer += dt;

                // 檢查哪些怪物時間到了
                for (auto it = m_PendingSubWaves.begin(); it != m_PendingSubWaves.end(); ) {
                    if (m_WaveTimer >= it->spawnDelay) {
                        // 根據路徑索引獲取座標點
                        const auto& route = currentMap->GetRouteByIndex(it->routeIndex);

                        // 生成怪物
                        for (auto& enemyType : it->enemies) {
                            m_Enemies.push_back(EnemyFactory::Create(enemyType, route));
                        }

                        // 生完後從待處理清單移除
                        it = m_PendingSubWaves.erase(it);
                    } else {
                        ++it;
                    }
                }

                // 如果怪生完了且畫面上清空了，就進入下一波
                if (m_PendingSubWaves.empty() && m_Enemies.empty()) {
                    m_IsWaveActive = false;
                    if (gm.GetCurrentWave() < static_cast<int>(m_Waves.size())) {
                        gm.NextWave();
                    } else {
                        m_VictoryMenu->SetVisible(true, gm.GetHealth());
                    }
                }
            }
        }

        // --- 更新與碰撞 ---
        m_TowerManager->UpdateAll(m_Enemies, m_Projectiles);

        for (auto it = m_Projectiles.begin(); it != m_Projectiles.end(); ) {
            (*it)->Update();
            if (!(*it)->IsActive()) it = m_Projectiles.erase(it);
            else ++it;
        }

        for (auto it = m_Enemies.begin(); it != m_Enemies.end(); ) {
            (*it)->Update();
            if ((*it)->ReachedEnd()) {
                gm.TakeDamage(1);
                it = m_Enemies.erase(it);
            } else if ((*it)->GetHP() <= 0 && (*it)->IsDeadAnimationFinished()) {
                gm.AddMoney((*it)->GetType() == Enemy::Type::GOBLIN ? 3 : 9);
                it = m_Enemies.erase(it);
            } else {
                ++it;
            }
        }

        // --- 交互邏輯 (滑鼠點擊、蓋塔、選單) ---
        glm::vec2 mousePos = Util::Input::GetCursorPosition();

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

    // --- 渲染 ---
    m_MapManager->Draw();
    for (auto& slot : m_TowerSlots) slot->Draw();
    m_TowerManager->DrawAll();
    for (auto& enemy : m_Enemies) enemy->Draw();
    for (auto& proj : m_Projectiles) proj->Draw();
    if (m_BuildMenu->IsVisible()) m_BuildMenu->Draw();
    if (m_Hud) m_Hud->Draw();
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