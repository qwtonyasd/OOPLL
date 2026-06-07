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

    m_UpgradeMenu = std::make_unique<UpgradeMenu>();

    // 🎯 載入音樂檔案資源
    m_BgmMainMenu   = std::make_shared<Util::BGM>("../PTSD/assets/audio/256_Music_MainMenu.mp3");
    m_BgmUnderAttack = std::make_shared<Util::BGM>("../PTSD/assets/audio/252_Music_UnderAttack.mp3");
    m_BgmEndGame    = std::make_shared<Util::BGM>("../PTSD/assets/audio/257_Music_EndGame.mp3");

    // 設定基礎音量 (0 ~ 100)
    m_BgmMainMenu->SetVolume(50);
    m_BgmUnderAttack->SetVolume(50);
    m_BgmEndGame->SetVolume(50);

    // 🎯 預設進入選擇關卡介面，播放 MainMenu 背景音樂
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
    int maxUnlocked = 5;
    int selected = m_WorldMap->Update(maxUnlocked);
    m_WorldMap->Draw(maxUnlocked);

    if (selected > 0) {
        LOG_INFO("Entering Level: {}", selected);
        ChangeLevel(selected);
        m_IsInGame = true;
        // 🎯 進入關卡戰鬥，切換為 UnderAttack 音樂
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

    // 🎯 核心修改一：即時戰敗攔截判定 (加入出怪與波次防護判定，防止剛進關卡時血量預設為 0 的空檔誤判)
    if (m_IsInGame && (m_IsWaveActive || !m_Enemies.empty()) && gm.GetHealth() <= 0 && !m_VictoryMenu->IsVisible()) {
        m_VictoryMenu->SetVisible(true, 0); // 傳入 0 觸發顯示失敗畫面 (21.png)
        ChangeMusic(MusicState::END_GAME);  // 切換至結算音樂
    }

    // 🎯 核心修改二：結算面板（不論勝敗）顯示時的「定格阻斷」邏輯
    if (m_VictoryMenu->IsVisible()) {
        m_VictoryMenu->Update();

        // 功能一：再打一次 (TRY AGAIN)
        if (m_VictoryMenu->IsRestartPressed()) {
            ChangeLevel(m_CurrentLevelID);       // 重新載入當前關卡
            ChangeMusic(MusicState::GAME_PLAY);  // 音樂切回 UnderAttack 戰鬥曲
            m_VictoryMenu->ResetFlags();
            return;
        }

        // 功能二：跳到主頁面 (QUIT)
        if (m_VictoryMenu->IsContinuePressed()) {
            m_IsInGame = false;                  // 離開遊戲關卡狀態
            m_VictoryMenu->SetVisible(false, 0); // 隱藏面板
            m_VictoryMenu->ResetFlags();
            ChangeMusic(MusicState::MAIN_MENU);  // 音樂切回主選單 MainMenu
            return;
        }

        return; // 🔥 阻斷點：面板顯示時直接返回，不讓下方的任何怪物與防禦塔執行 Update
    }

    // =========================================================================
    // 💡 遊戲核心更新迴圈（只有在活著且面板未開啟時，才會繼續執行）
    // =========================================================================

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
                if (gm.GetCurrentWave() < static_cast<int>(m_Waves.size())) {
                    gm.NextWave();
                } else {
                    // 通關所有波次，彈出勝利結算畫面
                    m_VictoryMenu->SetVisible(true, gm.GetHealth());
                    ChangeMusic(MusicState::END_GAME);
                }
            }
        }
    }

    m_TowerManager->UpdateAll(m_Enemies, m_Projectiles);

    // --- 2. 安全更新與清理階段 ---
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
                        gm.TakeDamage(0); // 扣血後如果歸零，下一影格最上方會立即捕捉定格
                    } else {
                        gm.AddMoney(enemy->GetType() == Enemy::Type::GOBLIN ? 3 : 9);
                    }
                }
                return shouldRemove;
            }),
        m_Enemies.end());

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
        // 優先處理 HUD 法術介面點擊
        SpellManager::SpellType clickedSpell = m_Hud->HandleClick(mousePos);
        if (clickedSpell != SpellManager::SpellType::NONE) {
            if (clickedSpell == SpellManager::SpellType::FIREBALL) m_SpellManager->SelectFireball();
            else if (clickedSpell == SpellManager::SpellType::REINFORCE) m_SpellManager->SelectReinforce();
        }
        // 處理施法點擊
        else if (m_SpellManager->GetSelectedSpell() != SpellManager::SpellType::NONE) {
            m_SpellManager->CastCurrentSpell(mousePos, m_Enemies, m_ActiveReinforcements);
        }
        else {
            bool actionHandled = false;

            // 1. 優先處理 ArcherTower 的技能購買點擊
            for (auto& tower : m_TowerManager->GetTowers()) {
                if (tower->GetIsSelected()) {
                    if (auto archer = std::dynamic_pointer_cast<ArcherTower>(tower)) {
                        int skillIdx = archer->GetClickedSkillIndex(mousePos); // 建議實作回傳 index 的函式
                        if (skillIdx != -1) {
                            archer->BuySkill(skillIdx);
                            actionHandled = true;
                            break;
                        }
                    }
                }
            }

            // 2. 處理升級按鈕點擊
            if (!actionHandled) {
                for (auto& tower : m_TowerManager->GetTowers()) {
                    if (tower->GetIsSelected() && tower->IsUpgradeClicked(mousePos)) {
                        int cost = tower->GetUpgradeCost();
                        if (gm.SpendMoney(cost)) tower->Upgrade();
                        actionHandled = true;
                        break;
                    }
                }
            }

            // 3. 處理塔的選擇或放置邏輯
            if (!actionHandled) {
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

    // 🎯 按下 BACKSPACE 放棄關卡回到選單時，也順便切回選單音樂
    if (Util::Input::IsKeyDown(Util::Keycode::BACKSPACE)) {
        m_IsInGame = false;
        ChangeMusic(MusicState::MAIN_MENU);
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

// 🎯 音訊狀態安全切換管理函式
void App::ChangeMusic(MusicState targetMusic) {
    if (m_CurrentMusicState == targetMusic) return;

    // 播放目標場景音樂
    switch (targetMusic) {
        case MusicState::MAIN_MENU:
            m_BgmMainMenu->Play();
            break;
        case MusicState::GAME_PLAY:
            m_BgmUnderAttack->Play();
            break;
        case MusicState::END_GAME:
            m_BgmEndGame->Play();
            break;
    }

    m_CurrentMusicState = targetMusic;
}

void App::End() {
    LOG_TRACE("App Terminated");
}