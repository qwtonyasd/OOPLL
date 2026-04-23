#include "App.hpp"
#include "MapFactory.hpp"
#include "EnemyFactory.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Time.hpp"
#include "Util/Logger.hpp"
#include "GameManager.hpp"
#include "VictoryMenu.hpp"

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

    // 1. 勝利選單邏輯
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
        // 2. 遊戲核心流程
        int currentWaveIdx = gm.GetCurrentWave() - 1;
        if (currentWaveIdx < static_cast<int>(m_Waves.size())) {
            auto& currentWave = m_Waves[currentWaveIdx];

            if (!m_IsWaveActive) {
                if (m_Enemies.empty()) {
                    m_WaveBreakTimer += dt;
                    if (m_WaveBreakTimer >= 3.0f) {
                        m_IsWaveActive = true;
                        m_SpawnIndex = 0;
                        m_WaveBreakTimer = 0.0f;
                    }
                }
            } else {
                m_SpawnTimer += dt;
                if (m_SpawnTimer >= 1.2f && static_cast<size_t>(m_SpawnIndex) < currentWave.enemyList.size()) {
                    const auto& route = currentMap->GetRandomRoute();
                    m_Enemies.push_back(EnemyFactory::Create(currentWave.enemyList[m_SpawnIndex], route));
                    m_SpawnIndex++;
                    m_SpawnTimer = 0.0f;
                }

                if (static_cast<size_t>(m_SpawnIndex) >= currentWave.enemyList.size() && m_Enemies.empty()) {
                    m_IsWaveActive = false;
                    if (gm.GetCurrentWave() < gm.GetTotalWaves()) {
                        gm.NextWave();
                    } else {
                        m_VictoryMenu->SetVisible(true, gm.GetHealth());
                    }
                }
            }
        }

        // 3. 更新所有塔與移動物
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

        // ==========================================
        // 4. 交互邏輯 (重點修改區塊)
        // ==========================================
        glm::vec2 mousePos = Util::Input::GetCursorPosition();

        if (m_BuildMenu->IsVisible()) {
            Tower::Type selectedType = m_BuildMenu->Update();
            if (selectedType != Tower::Type::NONE) {
                if (m_SelectedSlot) {
                    int cost = Tower::GetBaseCost(selectedType);
                    if (gm.SpendMoney(cost)) {
                        std::vector<glm::vec2> route = (!currentMap->GetRoutes().empty()) ? currentMap->GetRoutes()[0] : std::vector<glm::vec2>{};
                        m_TowerManager->AddTower(selectedType, m_SelectedSlot->GetPosition(), route);
                        m_SelectedSlot->SetOccupied(true);
                    }
                }
                m_BuildMenu->SetVisible(false);
                m_SelectedSlot = nullptr;
            } else if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
                // 點擊 BuildMenu 以外的地方則關閉選單
                if (glm::distance(mousePos, m_BuildMenu->GetTransform().translation) > 100.0f) {
                    m_BuildMenu->SetVisible(false);
                    m_SelectedSlot = nullptr;
                }
            }
        }
        else if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            // A. 優先檢查是否點擊到「已存在的塔」來顯示範圍
            if (m_TowerManager->HandleClick(mousePos)) {
                // 如果點到了塔，HandleClick 內部會處理 SetSelected(true)
                LOG_DEBUG("Tower selected via Mouse Click.");
            }
            else {
                // B. 如果沒點到塔，檢查是否點擊到「空的塔位」
                bool hitSlot = false;
                for (auto& slot : m_TowerSlots) {
                    if (!slot->IsOccupied() && glm::distance(mousePos, slot->GetPosition()) < 50.0f) {
                        m_SelectedSlot = slot;
                        m_BuildMenu->SetPosition(slot->GetPosition());
                        m_BuildMenu->SetVisible(true);

                        // 開啟蓋塔選單時，隱藏其他塔的範圍圈
                        m_TowerManager->ClearSelection();
                        hitSlot = true;
                        break;
                    }
                }

                // C. 如果點到純地圖背景（沒點到塔也沒點到塔位）
                if (!hitSlot) {
                    m_TowerManager->ClearSelection();
                }
            }
        }
    }

    // 5. 渲染順序
    m_MapManager->Draw();
    for (auto& slot : m_TowerSlots) slot->Draw();
    m_TowerManager->DrawAll(); // 這會呼叫各個 Tower 的 Draw()，顯示範圍圈與選單
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

    GameManager::GetInstance().InitLevel(265, 20);
    InitWaveData();

    m_TowerManager->Clear();
    m_Projectiles.clear();
    m_Enemies.clear();
    m_TowerSlots.clear();

    auto currentMap = m_MapManager->GetCurrentMap();
    if (currentMap) {
        for (const auto& pos : currentMap->GetTowerSlots()) {
            m_TowerSlots.push_back(std::make_shared<TowerSlot>(pos));
        }
    }

    m_BuildMenu->SetVisible(false);
    if (m_VictoryMenu) {
        m_VictoryMenu->SetVisible(false, 0);
        m_VictoryMenu->ResetFlags();
    }
}

// InitWaveData(), End() 等其餘函式保持不變...
void App::InitWaveData() {
    m_Waves.clear();
    m_Waves.push_back({{Enemy::Type::GOBLIN, Enemy::Type::GOBLIN, Enemy::Type::GOBLIN}});
    m_Waves.push_back({std::vector<Enemy::Type>(6, Enemy::Type::GOBLIN)});
    m_Waves.push_back({std::vector<Enemy::Type>(9, Enemy::Type::GOBLIN)});
    m_Waves.push_back({{Enemy::Type::GOBLIN, Enemy::Type::GOBLIN, Enemy::Type::GOBLIN, Enemy::Type::GOBLIN, Enemy::Type::ORC}});
    m_Waves.push_back({{Enemy::Type::ORC, Enemy::Type::ORC, Enemy::Type::ORC}});

    WaveConfig w6;
    for(int i=0; i<10; i++) w6.enemyList.push_back(Enemy::Type::GOBLIN);
    for(int i=0; i<4; i++) w6.enemyList.push_back(Enemy::Type::ORC);
    m_Waves.push_back(w6);

    m_Waves.push_back({std::vector<Enemy::Type>(16, Enemy::Type::GOBLIN)});

    m_SpawnIndex = 0;
    m_SpawnTimer = 0.0f;
    m_WaveBreakTimer = 0.0f;
    m_IsWaveActive = false;
}

void App::End() {
    LOG_TRACE("App Terminated");
}