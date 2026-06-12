#include "App.hpp"
#include "MapFactory.hpp"
#include "EnemyFactory.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Time.hpp"
#include "Util/Logger.hpp"
#include "GameManager.hpp"
#include "Egg.hpp" // 修正：加入 Egg 標頭檔以支援 ClearSpawnQueue

void App::Start() {
    LOG_TRACE("App Started");
    m_MapManager = std::make_unique<MapManager>();
    m_WorldMap = std::make_unique<WorldMap>();
    m_TowerManager = std::make_unique<TowerManager>(m_Root);
    m_BuildMenu = std::make_unique<BuildMenu>();
    m_Hud = std::make_unique<Hud>();
    m_SpellManager = std::make_unique<SpellManager>();

    m_VictoryMenu = std::make_unique<VictoryMenu>();
    m_Root.AddChild(m_VictoryMenu->GetGameObject());

    m_UpgradeMenu = std::make_unique<UpgradeMenu>();

    // 初始化 5 個關卡的狀態
    m_LevelProgress.resize(5);
    m_LevelProgress[0].unlocked = true; // 預設第一關解鎖
    for (size_t i = 1; i < 5; ++i) {
        m_LevelProgress[i].unlocked = false;
        m_LevelProgress[i].stars = 0;
    }

    m_BgmMainMenu   = std::make_shared<Util::BGM>("../PTSD/assets/audio/256_Music_MainMenu.mp3");
    m_BgmUnderAttack = std::make_shared<Util::BGM>("../PTSD/assets/audio/252_Music_UnderAttack.mp3");
    m_BgmEndGame    = std::make_shared<Util::BGM>("../PTSD/assets/audio/257_Music_EndGame.mp3");

    m_BgmMainMenu->SetVolume(50);
    m_BgmUnderAttack->SetVolume(50);
    m_BgmEndGame->SetVolume(50);

    m_BgmMainMenu->Play();
    m_CurrentMusicState = MusicState::MAIN_MENU;

    m_IsInGame = false;
    m_CurrentState = State::UPDATE;
}

void App::Update() {
    if (m_UpgradeMenu && m_UpgradeMenu->IsVisible()) {
        float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;
        if (dt > 0.1f) dt = 0.016f;

        m_UpgradeMenu->Update(dt);
        m_UpgradeMenu->Draw();

        if (Util::Input::IsKeyDown(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
            m_CurrentState = State::END;
        }
        return;
    }

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
    // 傳入完整的關卡狀態 vector
    int selected = m_WorldMap->Update(m_LevelProgress);

    if (selected > 0) {
        LOG_INFO("Entering Level: {}", selected);
        ChangeLevel(selected);
        m_IsInGame = true;
        ChangeMusic(MusicState::GAME_PLAY);
    }
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

    // 1. 即時戰敗處理
    if (m_IsInGame && (m_IsWaveActive || !m_Enemies.empty()) && gm.GetHealth() <= 0 && !m_VictoryMenu->IsVisible()) {
        m_VictoryMenu->SetVisible(true, 0);
        ChangeMusic(MusicState::END_GAME);
    }

    // 2. 結算面板點擊響應
    if (m_VictoryMenu->IsVisible()) {
        m_VictoryMenu->Update();

        if (m_VictoryMenu->IsRestartPressed()) {
            ChangeLevel(m_CurrentLevelID);
            ChangeMusic(MusicState::GAME_PLAY);
            m_VictoryMenu->ResetFlags();
            return;
        }

        if (m_VictoryMenu->IsContinuePressed()) {
            m_IsInGame = false;
            m_VictoryMenu->SetVisible(false, 0);
            m_VictoryMenu->ResetFlags();
            ChangeMusic(MusicState::MAIN_MENU);

            // 🎯 重置大地圖旗幟動畫計時器，讓動畫重新完整播放一次
            m_WorldMap->ResetAnimation();
            return;
        }

        goto RENDER_STAGE;
    }

    // =========================================================================
    // 💡 遊戲主更新邏輯
    // =========================================================================
    m_SpellManager->Update(dt, m_Enemies);

    {
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

                // 🎉 通關所有波次判定 🎉
                if (m_PendingSubWaves.empty() && m_Enemies.empty()) {
                    m_IsWaveActive = false;
                    if (gm.GetCurrentWave() < static_cast<int>(m_Waves.size())) {
                        gm.NextWave();
                    } else {
                        // 🎯 18血以上(含)皆為 3 星的評等邏輯
                        int finalHP = gm.GetHealth();
                        int calculatedStars = 1;

                        if (finalHP >= 18) {
                            calculatedStars = 3;      // 18、19、20 血都是 3 星
                        }
                        else if (finalHP >= 10) {
                            calculatedStars = 2;      // 10 ~ 17 血為 2 星
                        }
                        else {
                            calculatedStars = 1;      // 1 ~ 9 血為 1 星
                        }

                        // 更新當前關卡紀錄（若這次打得更好則覆蓋星數）
                        int idx = m_CurrentLevelID - 1;
                        if (calculatedStars > m_LevelProgress[idx].stars) {
                            m_LevelProgress[idx].stars = calculatedStars;
                        }

                        // 解鎖下一關
                        if (m_CurrentLevelID < 5) {
                            m_LevelProgress[m_CurrentLevelID].unlocked = true;
                        }

                        m_VictoryMenu->SetVisible(true, finalHP);
                        ChangeMusic(MusicState::END_GAME);
                    }
                }
            }
        }
    }

    m_TowerManager->UpdateAll(m_Enemies, m_Projectiles);

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

    m_Projectiles.erase(
        std::remove_if(m_Projectiles.begin(), m_Projectiles.end(),
            [&](auto& p) { p->Update(); return !p->IsActive(); }),
        m_Projectiles.end());

    m_Enemies.erase(
        std::remove_if(m_Enemies.begin(), m_Enemies.end(),
            [&](auto& enemy) {
                enemy->Update(m_Enemies, dt);
                bool shouldRemove = enemy->ReachedEnd() ||
                                   (enemy->GetHP() <= 0 && enemy->IsDeadAnimationFinished());
                if (shouldRemove) {
                    if (enemy->ReachedEnd()) {
                        int damageToTake = 1;
                        switch (enemy->GetType()) {
                            case Enemy::Type::OGRE:
                            case Enemy::Type::MARAUDER:
                                damageToTake = 3;
                                break;
                            case Enemy::Type::SPIDER_MATRIARCH:
                                damageToTake = 2;
                                break;
                            default:
                                damageToTake = 1;
                                break;
                        }
                        gm.TakeDamage(damageToTake);
                    } else {
                        gm.AddMoney(enemy->GetType() == Enemy::Type::GOBLIN ? 3 : 9);
                    }
                }
                return shouldRemove;
            }),
        m_Enemies.end());

    Egg::ClearSpawnQueue(m_Enemies);

    // 輸入控制管理
    {
        glm::vec2 mousePos = Util::Input::GetCursorPosition();
        // 🟢 修正：新增空白鍵「一鍵通關」測試外掛功能
        if (Util::Input::IsKeyDown(Util::Keycode::SPACE) && !m_VictoryMenu->IsVisible()) {
            LOG_INFO("【測試外掛】按下空白鍵，強制通關當前第 {} 關！", m_CurrentLevelID);

            // 1. 斬草除根：清空所有怪活動物與即將出生的佇列
            m_Enemies.clear();
            m_PendingSubWaves.clear();
            Egg::s_SpawnQueue.clear();
            m_IsWaveActive = false;


            //三星通關
            int calculatedStars = 3;




            // 3. 更新大世界進度紀錄
            int idx = m_CurrentLevelID - 1;
            if (calculatedStars > m_LevelProgress[idx].stars) {
                m_LevelProgress[idx].stars = calculatedStars;
            }

            // 4. 解鎖下一關
            if (m_CurrentLevelID < 5) {
                m_LevelProgress[m_CurrentLevelID].unlocked = true;
            }

            // 5. 直接彈出勝利面板與播放通關音樂
            m_VictoryMenu->SetVisible(true, 20);
            ChangeMusic(MusicState::END_GAME);
        }

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
                bool towerMenuHandled = false;

                for (auto& tower : m_TowerManager->GetTowers()) {
                    if (tower->GetIsSelected()) {

                        // 1. 【優先判定】檢查有沒有點擊到 Level 4 的技能圖示
                        if (tower->IsSkillClicked(mousePos)) {
                            int skillIdx = tower->GetClickedSkillIndex(mousePos);
                            if (skillIdx != -1) {
                                tower->BuySkill(skillIdx);
                            }
                            towerMenuHandled = true;
                            break;
                        }

                        // 2. 【次要判定】原本的升級按鈕點擊
                        if (tower->IsUpgradeClicked(mousePos)) {
                            int cost = tower->GetUpgradeCost();
                            if (gm.SpendMoney(cost)) tower->Upgrade();
                            towerMenuHandled = true;
                            break;
                        }

                        if (tower->IsSellClicked(mousePos)) {
                            // 1. 計算退款並透過 GameManager 加錢
                            int refund = tower->GetSellRefund();
                            gm.AddMoney(refund);
                            LOG_INFO("Tower Sold! Refunded {} gold.", refund);

                            // 2. 將對應的 TowerSlot 插槽還原為「未佔用」狀態
                            for (auto& slot : m_TowerSlots) {
                                if (glm::distance(slot->GetPosition(), tower->GetPosition()) < 1.0f) {
                                    slot->SetOccupied(false);
                                    break;
                                }
                            }

                            // 3. 從 TowerManager 中將這座塔擦除（因為後面會直接 break 結束迴圈，此處直接 erase 是安全的）
                            auto& towers = m_TowerManager->GetTowers();
                            towers.erase(std::remove(towers.begin(), towers.end(), tower), towers.end());

                            m_TowerManager->ClearSelection();
                            towerMenuHandled = true;
                            break;
                        }

                    }
                }

                // 3. 如果沒有點到任何防禦塔的 UI，才去判定點擊空地或切換選取
                if (!towerMenuHandled) {
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

RENDER_STAGE:
    m_MapManager->Draw();
    for (auto& slot : m_TowerSlots) slot->Draw();
    m_TowerManager->DrawAll();
    for (auto& soldier : m_ActiveReinforcements) if (soldier) soldier->Draw();
    for (auto& enemy : m_Enemies) enemy->Draw();
    for (auto& proj : m_Projectiles) proj->Draw();
    m_SpellManager->Draw();
    if (m_BuildMenu->IsVisible()) m_BuildMenu->Draw();
    if (m_Hud) m_Hud->Draw(m_SpellManager);

    if (m_VictoryMenu->IsVisible()) {
        m_VictoryMenu->Draw();
    }

    if (Util::Input::IsKeyDown(Util::Keycode::BACKSPACE)) {
        m_IsInGame = false;
        ChangeMusic(MusicState::MAIN_MENU);

        // 🎯 中途強制退出回到大地圖時，也重置動畫
        m_WorldMap->ResetAnimation();
    }
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

void App::ChangeMusic(MusicState targetMusic) {
    if (m_CurrentMusicState == targetMusic) return;

    switch (targetMusic) {
        case MusicState::MAIN_MENU: m_BgmMainMenu->Play(); break;
        case MusicState::GAME_PLAY: m_BgmUnderAttack->Play(); break;
        case MusicState::END_GAME:  m_BgmEndGame->Play(); break;
    }
    m_CurrentMusicState = targetMusic;
}

void App::End() {
    LOG_TRACE("App Terminated");
}