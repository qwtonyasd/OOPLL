#include "App.hpp"
#include "MapFactory.hpp"
#include "EnemyFactory.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

void App::Start() {
    LOG_TRACE("App Started");
    m_MapManager = std::make_unique<MapManager>();
    m_WorldMap = std::make_unique<WorldMap>();
    m_TowerManager = std::make_unique<TowerManager>();
    m_BuildMenu = std::make_unique<BuildMenu>();

    m_IsInGame = false;
    m_CurrentState = State::UPDATE;
}

void App::Update() {
    if (!m_IsInGame) {
        HandleSelectLevel();
    } else {
        HandleGamePlay();
    }

    // 處理退出遊戲與 ESC 鍵
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }
}

void App::HandleSelectLevel() {
    int selected = m_WorldMap->Update(m_MaxUnlockedLevel);
    if (selected > 0) {
        m_CurrentLevelId = selected;
        ChangeLevel(selected);
        m_IsInGame = true;
    }
}

void App::HandleGamePlay() {
    m_MapManager->Draw();

    glm::vec2 mousePos = Util::Input::GetCursorPosition();

    // 1. 繪製所有塔位
    for (auto& slot : m_TowerSlots) {
        slot->Draw();
    }

    // 2. 處理蓋塔 UI 與點擊邏輯
    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {

        // 如果選單已經打開，進行精確範圍偵測
        if (m_BuildMenu->IsVisible()) {
            Tower::Type type = m_BuildMenu->GetSelectedType();

            if (type != Tower::Type::NONE) {
                // 真正建造塔，並標記塔位已佔用
                m_TowerManager->AddTower(type, m_ActiveSlot->m_Transform.translation);
                m_ActiveSlot->SetOccupied(true);
                m_BuildMenu->Hide();
                m_ActiveSlot = nullptr;
            } else {
                // 點擊選單以外的地方則關閉選單
                m_BuildMenu->Hide();
                m_ActiveSlot = nullptr;
            }
        }
        // 點擊空塔位開啟選單
        else {
            for (auto& slot : m_TowerSlots) {
                if (!slot->IsOccupied() && glm::distance(mousePos, slot->m_Transform.translation) < 40.0f) {
                    m_ActiveSlot = slot;
                    m_BuildMenu->Show(slot->m_Transform.translation);
                    break;
                }
            }
        }
    }

    // 右鍵快速取消選單
    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_RB)) {
        m_BuildMenu->Hide();
    }

    // 3. 繪製塔與 UI
    m_TowerManager->UpdateAll(m_Enemies);
    m_TowerManager->DrawAll();

    if (m_BuildMenu->IsVisible()) {
        m_BuildMenu->Draw();
    }

    // 4. 更新敵人邏輯
    for (auto it = m_Enemies.begin(); it != m_Enemies.end(); ) {
        (*it)->Update();
        (*it)->Draw();
        if ((*it)->IsDead()) it = m_Enemies.erase(it);
        else ++it;
    }

    // 測試：按 E 生成敵人
    if (Util::Input::IsKeyDown(Util::Keycode::E)) {
        auto currentMap = m_MapManager->GetCurrentMap();
        if (currentMap) {
            const auto& route = currentMap->GetRandomRoute();
            m_Enemies.push_back(EnemyFactory::Create(EnemyFactory::Type::GOBLIN, route));
        }
    }

    // 返回選單：按 Backspace
    if (Util::Input::IsKeyDown(Util::Keycode::BACKSPACE)) {
        m_IsInGame = false;
        m_BuildMenu->Hide();
    }
}

void App::ChangeLevel(int levelId) {
    (void)levelId; // 消除警告
    auto newMap = MapFactory::CreateLevel(levelId);

    m_MapManager->AddLevel(levelId, newMap);
    m_MapManager->SwitchLevel(levelId);

    // 清空舊關卡資料
    m_TowerManager->Clear();
    m_TowerSlots.clear();
    m_Enemies.clear();

    // 重新載入塔位物件
    auto currentMap = m_MapManager->GetCurrentMap();
    if (currentMap) {
        for (const auto& pos : currentMap->GetTowerSlots()) {
            m_TowerSlots.push_back(std::make_shared<TowerSlot>(pos));
        }
    }
}

void App::End() {
    LOG_TRACE("App Ended");
}