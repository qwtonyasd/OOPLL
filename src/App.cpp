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

    // 1. 初始化法術管理器
    m_SpellManager = std::make_unique<SpellManager>();

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
    int maxUnlocked = 5;
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
                    if (gm.GetCurrentWave() < static_cast<int>(m_Waves.size())) {
                        gm.NextWave();
                    } else {
                        m_VictoryMenu->SetVisible(true, gm.GetHealth());
                    }
                }
            }
        }

        m_TowerManager->UpdateAll(m_Enemies, m_Projectiles);

        // 更新援軍小兵 (已修正參數)
        for (auto it = m_ActiveReinforcements.begin(); it != m_ActiveReinforcements.end(); ) {
            auto& soldier = *it;
            if (soldier) {
                soldier->Update(m_Enemies, dt);
                if (soldier->GetHP() <= 0 && soldier->IsDeadAnimationFinished()) {
                    soldier->ReleaseEnemy();
                    it = m_ActiveReinforcements.erase(it);
                    continue;
                }
            }
            ++it;
        }

        for (auto it = m_Projectiles.begin(); it != m_Projectiles.end(); ) {
            (*it)->Update();
            if (!(*it)->IsActive()) it = m_Projectiles.erase(it);
            else ++it;
        }

        // 更新敵人 (已修正參數)
        for (auto it = m_Enemies.begin(); it != m_Enemies.end(); ) {
            (*it)->Update(m_Enemies, dt); // 補上了 m_Enemies 與 dt

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

        glm::vec2 mousePos = Util::Input::GetCursorPosition();

        if (Util::Input::IsKeyDown(Util::Keycode::NUM_1)) m_SpellManager->SelectFireball();
        if (Util::Input::IsKeyDown(Util::Keycode::NUM_2)) m_SpellManager->SelectReinforce();

        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_RB)) {
            m_SpellManager->CancelSelection();
        }

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
            }
            else if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
                if (glm::distance(mousePos, m_BuildMenu->GetTransform().translation) > 100.0f) {
                    m_BuildMenu->SetVisible(false);
                    m_SelectedSlot = nullptr;
                }
            }
        }
        else if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            SpellManager::SpellType clickedSpell = m_Hud->HandleClick(mousePos);

            if (clickedSpell != SpellManager::SpellType::NONE) {
                if (clickedSpell == SpellManager::SpellType::FIREBALL) {
                    m_SpellManager->SelectFireball();
                }
                else if (clickedSpell == SpellManager::SpellType::REINFORCE) {
                    m_SpellManager->SelectReinforce();
                }
            }
            else if (m_SpellManager->GetSelectedSpell() != SpellManager::SpellType::NONE) {
                m_SpellManager->CastCurrentSpell(mousePos, m_Enemies, m_ActiveReinforcements);
            }
            else {
                bool upgradeHandled = false;
                for (auto& tower : m_TowerManager->GetTowers()) {
                    if (tower->GetIsSelected() && tower->IsUpgradeClicked(mousePos)) {
                        int cost = tower->GetUpgradeCost();
                        if (gm.SpendMoney(cost)) {
                            tower->Upgrade();
                        }
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
                        if (!hitSlot) {
                            m_TowerManager->ClearSelection();
                        }
                    }
                }
            }
        }
    }

    m_MapManager->Draw();
    for (auto& slot : m_TowerSlots) slot->Draw();
    m_TowerManager->DrawAll();

    for (auto& soldier : m_ActiveReinforcements) {
        if (soldier) soldier->Draw();
    }

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

    // 換關卡時，清空場上殘留的法術小兵與選取狀態
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