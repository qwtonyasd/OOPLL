#ifndef GAME_DATA_HPP
#define GAME_DATA_HPP

#include <cmath>
#include <vector>

class GameData {
public:
    static GameData& GetInstance() {
        static GameData instance;
        return instance;
    }

    // 儲存 6 種升級項的等級 (0-5)
    // 預設對應：0:箭塔, 1:法師塔, 2:兵營, 3:砲塔, 4:火球術, 5:援軍
    int talentLevels[6] = {0, 0, 0, 0, 0, 0};
    int totalStars = 15;

    // --- 統一的計算接口 (保留給法師塔、兵營等使用) ---
    float GetMultiplier(int routeIndex, int level) {
        switch (routeIndex) {
            case 0:
                // 箭塔的升級包含退費、範圍與傷害，已改由下方專屬函式處理
                return 1.0f;
            case 1: // 法師塔
                return std::pow(1.12f, level);
            case 2: // 兵營
                return 1.0f + (level * 0.15f);
            // ... 繼續補完其他 3 個 case
            default:
                return 1.0f;
        }
    }

    // ==========================================
    // 🏹 針對箭塔的特定邏輯 (階梯式升級)
    // ==========================================

    // 💰 等級 1 開始，蓋塔返還 20 金幣
    int GetArcherRefund(int lv) {
        return (lv >= 1) ? 20 : 0;
    }

    // 🎯 範圍加成邏輯 (Lv2 -> +8%, Lv4 -> 再 +5%)
    float GetArcherRange(int lv) {
        switch (lv) {
            case 0: return 1.00f;
            case 1: return 1.00f;
            case 2: return 1.08f;          // 基礎增加 8%
            case 3: return 1.08f;          // 維持增加 8%
            case 4: return 1.08f * 1.05f;  // 在 8% 基礎上再疊加 5% (約 1.134x)
            case 5: return 1.08f * 1.05f;  // 維持 Lv4 狀態
            default: return 1.00f;
        }
    }

    // ⚔️ 傷害加成邏輯 (Lv3 -> +10%, Lv5 -> 再 +10%)
    float GetArcherDamage(int lv) {
        switch (lv) {
            case 0: return 1.00f;
            case 1: return 1.00f;
            case 2: return 1.00f;
            case 3: return 1.10f;          // 基礎增加 10%
            case 4: return 1.10f;          // 維持增加 10%
            case 5: return 1.10f * 1.10f;  // 在 10% 基礎上再疊加 10% (約 1.21x)
            default: return 1.00f;
        }
    }

    // ==========================================
    // 🔮 針對法師塔的特定邏輯 (精準階梯式升級)
    // ==========================================

    // 🎯 魔法塔的本體射程範圍加成
    float GetMageAttackRange(int lv) {
        if (lv >= 5) return 1.1f * 1.05f * 1.05f; // Lv5：在 Lv3 的基礎上再提升 1.05 倍
        if (lv >= 3) return 1.1f * 1.05f;        // Lv3、Lv4：範圍提升 1.1 * 1.05 倍
        if (lv >= 1) return 1.1f;               // Lv1、Lv2：範圍提升 1.1 倍
        return 1.0f;                             // Lv0：白板
    }

    // ⚔️ 魔法塔的攻擊力（傷害）加成
    float GetMageDamage(int lv) {
        if (lv >= 5) return 1.1f * 1.1f * 1.05f; // Lv5：在 Lv4 的基礎上再提升 1.05 倍
        if (lv >= 4) return 1.1f * 1.1f;        // Lv4：攻擊力提升 1.1 * 1.1 倍
        if (lv >= 2) return 1.1f;               // Lv2、Lv3：攻擊力提升 1.1 倍
        return 1.0f;                             // Lv0、Lv1：白板
    }

    // ==========================================
    // 🛡️ 針對兵營的特定邏輯 (精準階梯式升級)
    // ==========================================

    // 🩸 兵營小兵的生命值加成
    float GetBarrackHP(int lv) {
        if (lv >= 5) return 1.1f * 1.1f * 1.05f; // Lv5：在 Lv3 的基礎上再提升 1.05 倍
        if (lv >= 3) return 1.1f * 1.1f;        // Lv3、Lv4：生命值提升 1.1 * 1.1 倍
        if (lv >= 1) return 1.1f;               // Lv1、Lv2：生命值提升 1.1 倍
        return 1.0f;                             // Lv0：白板
    }

    // ⚔️ 兵營小兵的攻擊力加成
    float GetBarrackDamage(int lv) {
        if (lv >= 5) return 1.1f * 1.1f * 1.05f; // Lv5：在 Lv4 的基礎上再提升 1.05 倍
        if (lv >= 4) return 1.1f * 1.1f;        // Lv4：攻擊力提升 1.1 * 1.1 倍
        if (lv >= 2) return 1.1f;               // Lv2、Lv3：攻擊力提升 1.1 倍
        return 1.0f;                             // Lv0、Lv1：白板
    }

    // ==========================================
    // 💣 針對砲塔的特定邏輯 (精準階梯式升級)
    // ==========================================

    // 💥 爆炸範圍加成 (1等開始就提升 1.1 倍)
    float GetBombSplashRadius(int lv) {
        return (lv >= 1) ? 1.1f : 1.0f;
    }

    // 🎯 砲塔本體的射程範圍加成 (2等提升 1.1 倍，5等變 1.1 * 1.1 倍)
    float GetBombAttackRange(int lv) {
        if (lv >= 5) return 1.1f * 1.1f;
        if (lv >= 2) return 1.1f;
        return 1.0f;
    }

    // ⚔️ 攻擊力加成 (3等提升 1.1 倍，4等以上變 1.1 * 1.1 倍)
    float GetBombDamage(int lv) {
        if (lv >= 4) return 1.1f * 1.1f;
        if (lv >= 3) return 1.1f;
        return 1.0f;
    }

private:
    GameData() = default;
};

#endif