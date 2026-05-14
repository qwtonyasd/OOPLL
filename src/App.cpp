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
        // --- 波次邏輯：直接使用從 Map 拿到的 m_Waves ---
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
                    // 使用 m_Waves.size() 動態判斷總波次
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

        // --- 交互邏輯 ---
        glm::vec2 mousePos = Util::Input::GetCursorPosition();

        if (m_BuildMenu->IsVisible()) {
            Tower::Type selectedType = m_BuildMenu->Update();
            if (selectedType != Tower::Type::NONE) {
                if (m_SelectedSlot) {
                    int cost = Tower::GetBaseCost(selectedType);
                    if (gm.SpendMoney(cost)) {
                        // 使用當前地圖的第一條路徑作為預設參考
                        std::vector<glm::vec2> route = (!currentMap->GetRoutes().empty()) ? currentMap->GetRoutes()[0] : std::vector<glm::vec2>{};
                        m_TowerManager->AddTower(selectedType, m_SelectedSlot->GetPosition(), route);
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
        }
        else if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            if (m_TowerManager->HandleClick(mousePos)) {
                LOG_DEBUG("Tower selected.");
            }
            else {
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

    // 1. 透過工廠獲取完整關卡資訊
    auto newMap = MapFactory::CreateLevel(levelId);

    // 2. 更新 MapManager
    m_MapManager->AddLevel(levelId, newMap);
    m_MapManager->SwitchLevel(levelId);

    // 3. 從 Map 物件同步波次劇本
    m_Waves = newMap->GetWaves();

    // 4. 初始化遊戲數值
    GameManager::GetInstance().InitLevel(265, 20);
    // 同步 GameManager 的總波次 (這很重要，勝利條件會用到)
    GameManager::GetInstance().SetTotalWaves(static_cast<int>(m_Waves.size()));

    // 5. 清理場地
    m_TowerManager->Clear();
    m_Projectiles.clear();
    m_Enemies.clear();
    m_TowerSlots.clear();

    // 6. 根據 Map 的配置生成塔位
    for (const auto& pos : newMap->GetTowerSlots()) {
        m_TowerSlots.push_back(std::make_shared<TowerSlot>(pos));
    }

    // 7. 重置波次狀態機
    m_SpawnIndex = 0;
    m_SpawnTimer = 0.0f;
    m_WaveBreakTimer = 0.0f;
    m_IsWaveActive = false;

    // 8. 重置選單狀態
    m_BuildMenu->SetVisible(false);
    if (m_VictoryMenu) {
        m_VictoryMenu->SetVisible(false, 0);
        m_VictoryMenu->ResetFlags();
    }
}

void App::End() {
    LOG_TRACE("App Terminated");
}