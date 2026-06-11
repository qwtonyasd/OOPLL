#ifndef LEVEL_STATUS_HPP
#define LEVEL_STATUS_HPP

struct LevelStatus {
    bool unlocked = false; // 該關卡是否已解鎖
    int stars = 0;         // 獲得星數 (0 代表未通關，1~3 代表通關星數)
};

#endif // LEVEL_STATUS_HPP