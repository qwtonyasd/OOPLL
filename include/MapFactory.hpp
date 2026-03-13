#ifndef MAP_FACTORY_HPP
#define MAP_FACTORY_HPP

#include "Map.hpp"

class MapFactory {
public:
    static std::shared_ptr<Map> CreateLevel(int levelId) {
        LevelConfig config;
        config.levelId = levelId;

        if (levelId == 1) {
            config.imagePath = "../PTSD/assets/sprites/images/287.png";
            // 設計 4 條路線
            config.routes = {
                { {-15, 450}, {-15, 180}, {-245, 10}, {550, -140} }, // 路線 1
                { {-100, 450}, {-100, 100}, {200, 100}, {550, 100} }, // 路線 2 (範例)
                { {100, 450}, {100, -100}, {550, -100} },             // 路線 3 (範例)
                { {-300, 200}, {0, 200}, {300, -200}, {550, -200} }   // 路線 4 (範例)
            };
            config.towerSlotPositions = { 
                {-110, 110}, {-187, 78}, {-101, -14}, {-102, -61}, {-21, -54}, {34, -149}, {120, -73}, {228, -149}
            };
        } 
        else if (levelId == 2) {
            config.imagePath = "../PTSD/assets/sprites/images/1615.png";
            config.routes = { /* 設定第 2 關的 4 條線 */ };
            config.towerSlotPositions = { {88, 146}, {90, 93} , {-54, 145}, {-105, 26}, {-49, -91}, {27, -73}, {36, -191}};
        }
        else if (levelId == 3) {
            config.imagePath = "../PTSD/assets/sprites/images/1583.png";
            config.routes = { /* 設定第 2 關的 4 條線 */ };
            config.towerSlotPositions = { {-26, 205}, {-88, 186},{-21, 96},{51, 110},{129, 115},{114, 13},{-81, -39},{-5, -68}, {67, -92},{225, -165},{-153, -139},{-67, -173}};
        }
        else if (levelId == 4) {
            config.imagePath = "../PTSD/assets/sprites/images/1576.png";
            config.routes = { /* 設定第 2 關的 4 條線 */ };
            config.towerSlotPositions = { {-265, -51}, {110, -201},{239, -180}, {-27, -85},{39, -97},{109, -94},{191, -56},{197, 16},{161, 81},{53, 163},{-16, 164},{-86, 152},{-137, 118},{-160, 47},{-137, 1} };
        }
        else if (levelId == 5) {
            config.imagePath = "../PTSD/assets/sprites/images/1573.png";
            config.routes = { /* 設定第 2 關的 4 條線 */ };
            config.towerSlotPositions = { {-119, -178},{-165, -43},{-96, 59},{-238, 124},{38, 198},{172, 104}, {216, 67},{5, -33},{6, -82},{134, -35},{120, -80},{114, -137}};
        }


        return std::make_shared<Map>(config);
    }
};

#endif