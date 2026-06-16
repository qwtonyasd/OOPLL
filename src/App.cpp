#include "App.hpp"
#include "MapFactory.hpp"
#include "EnemyFactory.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Time.hpp"
#include "Util/Logger.hpp"
#include "GameManager.hpp"
#include "Egg.hpp"

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

    m_LevelProgress.resize(5);
    m_LevelProgress[0].unlocked = true;
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
    int selected = m_WorldMap->Update(m_LevelProgress);

    if (selected > 0) {
        LOG_INFO("Entering Level: {}", selected);
        ChangeLevel(selected);
        m_IsInGame = true;
        ChangeMusic(MusicState::GAME_PLAY);
    }
    else if (selected == -1) {
        // 1. 計算目前累積的所有星星總數
        int totalStars = 0;
        for (const auto& lv : m_LevelProgress) {
            totalStars += lv.stars;
        }

        // 2. 將計算好的總星數傳入 UpgradeMenu
        m_UpgradeMenu->SetTotalEarnedStars(totalStars);

        LOG_INFO("Opening Upgrade Menu... Total Stars: {}", totalStars);
        m_UpgradeMenu->SetVisible(true);
    }
}

void App::HandleGamePlay() {
    auto& gm = GameManager::GetInstance();
    float dt = static_cast<float>(Util::Time::GetDeltaTimeMs()) / 1000.0f;
    if (dt > 0.1f) dt = 0.016f;
    auto currentMap = m_MapManager->GetCurrentMap();

    if (m_IsInGame && (m_IsWaveActive || !m_Enemies.empty()) && gm.GetHealth() <= 0 && !m_VictoryMenu->IsVisible()) {
        m_VictoryMenu->SetVisible(true, 0);
        ChangeMusic(MusicState::END_GAME);
    }

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
            m_WorldMap->ResetAnimation();
            return;
        }
        goto RENDER_STAGE;
    }

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

                if (m_PendingSubWaves.empty() && m_Enemies.empty()) {
                    m_IsWaveActive = false;
                    if (gm.GetCurrentWave() < static_cast<int>(m_Waves.size())) {
                        gm.NextWave();
                    } else {
                        int finalHP = gm.GetHealth();
                        int calculatedStars = 1;

                        if (finalHP >= 18)       calculatedStars = 3;
                        else if (finalHP >= 10)  calculatedStars = 2;
                        else                     calculatedStars = 1;

                        int idx = m_CurrentLevelID - 1;
                        if (calculatedStars > m_LevelProgress[idx].stars) {
                            m_LevelProgress[idx].stars = calculatedStars;
                        }

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

    // =================================================================
    // 🎯 核心重構區：安全拆分 Update、小蜘蛛注入、與容器過濾
    // =================================================================

    // 步驟 1：讓現有的所有怪物完整跑完本幀的邏輯更新（包含女皇產卵、蛋的孵化計時）
    for (auto& enemy : m_Enemies) {
        if (enemy) {
            enemy->Update(m_Enemies, dt);
        }
    }

    // 步驟 2：立刻釋放暫存佇列，將新孵化的小蜘蛛安全併入主追蹤清單中
    Egg::ClearSpawnQueue(m_Enemies);

    // 步驟 3：統一使用 std::remove_if 進行死屍與衝線怪物的過濾清理
    m_Enemies.erase(
        std::remove_if(m_Enemies.begin(), m_Enemies.end(),
            [&](auto& enemy) {
                bool reachedEnd = enemy->ReachedEnd();

                if (reachedEnd) {
                    int damageToTake = 1;
                    switch (enemy->GetType()) {
                        case Enemy::Type::OGRE:
                        case Enemy::Type::MARAUDER:         damageToTake = 3; break;
                        case Enemy::Type::SPIDER_MATRIARCH: damageToTake = 2; break;
                        default:                            damageToTake = 1; break;
                    }
                    gm.TakeDamage(damageToTake);
                    return true; // 標記移除
                }

                // 🎯 特殊處理：蜘蛛蛋的安全移除判定
                if (enemy->GetType() == Enemy::Type::EGG) {
                    auto eggPtr = std::dynamic_pointer_cast<Egg>(enemy);
                    if (eggPtr && eggPtr->IsEggReadyToRemove()) {
                        return true; // 時間到孵化完成，或者被打死，安全移除
                    }
                    return false;
                }

                // 常規怪物的死亡與擊殺獎勵金判定
                if (enemy->GetHP() <= 0) {
                    // 🎯 核心修正：血量歸零了，但必須等到「死亡動畫播完」
                    if (enemy->IsDeadAnimationFinished()) {
                        gm.AddMoney(enemy->GetType() == Enemy::Type::GOBLIN ? 3 : 9); // 真正移除時才給錢，避免重複加錢
                        return true; // 動畫播完了，安全功成身退，標記移除
                    }
                    return false; // 動畫還沒播完，這一幀先留著它，讓它繼續跑 Update 與 Draw
                }

                return false; // 存活怪物保留
            }),
        m_Enemies.end());


    // 輸入與測試外掛控制管理
    {
        if (Util::Input::IsKeyDown(Util::Keycode::R)) {
            GameManager::GetInstance().AddMoney(100);
            LOG_INFO("get 100 coin }");
        }

        glm::vec2 mousePos = Util::Input::GetCursorPosition();

        // 🟢 完美修正版：一鍵三星通關測試外掛 (安全解除所有士兵的目標指針，根治發呆 Bug)
        if (Util::Input::IsKeyDown(Util::Keycode::SPACE) && !m_VictoryMenu->IsVisible()) {
            LOG_INFO("【測試外掛】按下空白鍵，安全強制通關當前第 {} 關！", m_CurrentLevelID);

            for (auto& soldier : m_ActiveReinforcements) {
                if (soldier) {
                    soldier->ReleaseEnemy();
                    soldier->SetState(Soldier::State::MOVE_TO_RALLY);
                }
            }

            m_TowerManager->ClearSelection();
            m_Enemies.clear();
            m_PendingSubWaves.clear();
            Egg::s_SpawnQueue.clear();
            m_IsWaveActive = false;

            int calculatedStars = 3;
            int idx = m_CurrentLevelID - 1;
            if (calculatedStars > m_LevelProgress[idx].stars) {
                m_LevelProgress[idx].stars = calculatedStars;
            }

            if (m_CurrentLevelID < 5) {
                m_LevelProgress[m_CurrentLevelID].unlocked = true;
            }

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
                        if (tower->IsSkillClicked(mousePos)) {
                            int skillIdx = tower->GetClickedSkillIndex(mousePos);
                            if (skillIdx != -1) tower->BuySkill(skillIdx);
                            towerMenuHandled = true;
                            break;
                        }

                        if (tower->IsUpgradeClicked(mousePos)) {
                            int cost = tower->GetUpgradeCost();
                            if (gm.SpendMoney(cost)) tower->Upgrade();
                            towerMenuHandled = true;
                            break;
                        }

                        if (tower->IsSellClicked(mousePos)) {
                            int refund = tower->GetSellRefund();
                            gm.AddMoney(refund);

                            for (auto& slot : m_TowerSlots) {
                                if (glm::distance(slot->GetPosition(), tower->GetPosition()) < 1.0f) {
                                    slot->SetOccupied(false);
                                    break;
                                }
                            }

                            auto& towers = m_TowerManager->GetTowers();
                            towers.erase(std::remove(towers.begin(), towers.end(), tower), towers.end());
                            m_TowerManager->ClearSelection();
                            towerMenuHandled = true;
                            break;
                        }
                    }
                }

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
    if (m_VictoryMenu->IsVisible()) m_VictoryMenu->Draw();

    if (Util::Input::IsKeyDown(Util::Keycode::BACKSPACE)) {
        m_IsInGame = false;
        ChangeMusic(MusicState::MAIN_MENU);
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
    m_SpellManager->Reset();
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