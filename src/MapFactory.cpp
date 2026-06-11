#include "MapFactory.hpp"
#include <cstdlib>
#include <ctime>

std::shared_ptr<Map> MapFactory::CreateLevel(int levelId) {
    LevelConfig config;
    config.levelId = levelId;

    // 確保隨機種子只初始化一次
    static bool seeded = false;
    if (!seeded) {
        srand(static_cast<unsigned>(time(NULL)));
        seeded = true;
    }

    // 隨機選路輔助 lambda
    auto randomRoute = [](int start, int count) {
        return start + (rand() % count);
    };

    // ==================== Level 1 ====================
    if (levelId == 1) {
        config.initialMoney = 265;
        config.imagePath = "../PTSD/assets/sprites/images/Map/1.png";
        config.routes = {
            { {-50, 295}, {-44, 102}, {-71, 82}, {-152, 54},{-184, 13},{-203, -24},{-201, -74}, {-170, -112},{-117, -131},{-42, -137},{27, -125},{90, -140},{146, -149},{182, -118},{228, -82},{283, -72},{371, -75} },
            { {-34, 294}, {-38, 107}, {-52, 79}, {-156, 37}, {-175, 4}, {-183, -40}, {-183, -80}, {-148, -114}, {-91, -129}, {-29, -125}, {36, -114}, {97, -134}, {131, -145}, {162, -127}, {193, -94}, {232, -69}, {279, -61}, {366, -58} },
            { {-11, 305}, {-21, 124}, {-41, 79}, {-147, 39}, {-175, -16}, {-173, -67}, {-151, -99}, {-110, -118}, {-74, -121}, {14, -115}, {51, -112}, {86, -124}, {133, -133}, {169, -113}, {202, -84}, {235, -57}, {309, -51}, {401, -49} },
            { {0, 302}, {-2, 227}, {2, 128}, {-39, 61}, {-147, 17}, {-164, -28}, {-162, -64}, {-139, -101}, {-99, -105}, {-54, -111}, {-4, -108}, {33, -99}, {66, -108}, {98, -117}, {129, -119}, {161, -107}, {192, -79}, {219, -48}, {245, -37}, {297, -36}, {339, -36}, {385, -33} }
        };
        config.towerSlotPositions = { {-110, 110}, {-187, 78}, {-101, -14}, {-102, -61}, {-21, -54}, {34, -149}, {120, -73}, {228, -149} };

        // 輔助 lambda: 用來快速產生一波怪物
        auto addW = [&](const std::vector<Enemy::Type>& types, float interval) {
            WaveConfig wave; float d = 0;
            for (auto t : types) {
                wave.subWaves.emplace_back(randomRoute(0, 4), std::vector<Enemy::Type>{t}, d);
                d += interval;
            }
            config.waves.push_back(wave);
        };

        addW({Enemy::Type::GOBLIN, Enemy::Type::GOBLIN, Enemy::Type::GOBLIN}, 1.2f);

    }
    // ==================== Level 2 ====================
    else if (levelId == 2) {
        config.initialMoney = 220;
        config.imagePath = "../PTSD/assets/sprites/images/Map/2.png";
        config.routes = {
            { {337, 108}, {189, 113}, {180, 143}, {169, 176}, {160, 207}, {137, 225}, {114, 232}, {92, 238}, {70, 240}, {45, 231}, {32, 220}, {20, 206}, {13, 184}, {5, 168}, {3, 146}, {-1, 132}, {-10, 118}, {-28, 109}, {-52, 105}, {-86, 100}, {-110, 97}, {-143, 91}, {-166, 79}, {-183, 66}, {-193, 35}, {-192, 1}, {-182, -20}, {-159, -33}, {-135, -43}, {-106, -47}, {-82, -48}, {-64, -46}, {-43, -40}, {-15, -35}, {3, -30}, {36, -28}, {53, -32}, {78, -43}, {93, -68}, {91, -84}, {79, -96}, {65, -106}, {47, -112}, {13, -119}, {-11, -126}, {-42, -143}, {-54, -177}, {-54, -207}, {-54, -248}, {-60, -274}, {-67, -312}},
            {{344, 88}, {316, 88}, {290, 91}, {273, 92}, {256, 91}, {236, 90}, {215, 90}, {196, 93}, {183, 97}, {164, 120}, {155, 145}, {151, 163}, {145, 178}, {137, 189}, {129, 199}, {115, 208}, {85, 214}, {69, 215}, {54, 213}, {42, 203}, {36, 193}, {32, 178}, {27, 164}, {24, 148}, {17, 123}, {-29, 95}, {-56, 93}, {-67, 91}, {-83, 89}, {-107, 86}, {-126, 82}, {-136, 80}, {-151, 76}, {-160, 68}, {-166, 51}, {-171, 36}, {-173, 22}, {-173, 10}, {-169, 2}, {-164, -6}, {-157, -14}, {-144, -21}, {-123, -27}, {-101, -30}, {-72, -31}, {-48, -26}, {-37, -25}, {-22, -23}, {-2, -21}, {19, -20}, {33, -20}, {46, -20}, {61, -23}, {75, -28}, {90, -33}, {98, -41}, {103, -47}, {103, -63}, {102, -76}, {99, -84}, {95, -93}, {91, -101}, {80, -107}, {69, -113}, {57, -122}, {45, -127}, {32, -130}, {15, -133}, {4, -136}, {-8, -141}, {-16, -145}, {-21, -153}, {-26, -164}, {-30, -177}, {-33, -192}, {-34, -207}, {-35, -224}, {-37, -239}, {-36, -251}, {-35, -264}, {-35, -275}, {-35, -286}, {-36, -306}}
        };
        config.towerSlotPositions = { {88, 146}, {90, 93} , {-54, 145}, {-105, 26}, {-49, -91}, {27, -73}, {36, -191}};

        auto addW2 = [&](const std::vector<Enemy::Type>& types, float interval) {
            WaveConfig wave; float d = 0;
            for (auto t : types) {
                wave.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{t}, d);
                d += interval;
            }
            config.waves.push_back(wave);
        };
        addW2(std::vector<Enemy::Type>(20, Enemy::Type::GOBLIN), 0.5f);
        addW2(std::vector<Enemy::Type>(24, Enemy::Type::GOBLIN), 0.5f);

        WaveConfig w3; float d3 = 0;
        for(int i=0; i<4; i++) { w3.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::ORC}, d3); d3 += 1.3f; }
        for(int i=0; i<12; i++) { w3.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::GOBLIN}, d3); d3 += 0.6f; }
        config.waves.push_back(w3);

        WaveConfig w4; float d4 = 0;
        for(int i = 0; i < 6; i++) {
            w4.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::WULF}, d4);
            d4 += 0.4f;
        }
        config.waves.push_back(w4);



        WaveConfig w5; float d5 = 0;
        for(int i=0; i<6; i++) { w5.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::ORC}, d5); d5 += 1.2f; }
        for(int i=0; i<12; i++) { w5.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::GOBLIN}, d5); d5 += 0.6f; }
        config.waves.push_back(w5);

        WaveConfig w6; float d6 = 0;
        for(int i=0; i<6; i++) { w6.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::ORC}, d6); d6 += 1.0f; }
        for(int i=0; i<6; i++) { w6.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::WULF}, d6); d6 += 0.5f; }
        for(int i=0; i<10; i++) { w6.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::GOBLIN}, d6); d6 += 0.6f; }
        config.waves.push_back(w6);

        WaveConfig w7; float d7 = 0;
        for(int i=0; i<10; i++) { w7.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::ORC}, d7); d7 += 0.9f; }
        for(int i=0; i<10; i++) { w7.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::WULF}, d7); d7 += 0.4f; }
        for(int i=0; i<10; i++) { w7.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::GOBLIN}, d7); d7 += 0.5f; }
        config.waves.push_back(w7);
    }
    // ==================== Level 3 ====================
    else if (levelId == 3) {
        config.initialMoney = 300; // 已改為 300
        config.imagePath = "../PTSD/assets/sprites/images/Map/3.png";
        config.routes = {
            { {364, 161},{337, 162}, {312, 164}, {276, 168}, {239, 171}, {208, 172}, {118, 171}, {66, 167}, {27, 165}, {-17, 158}, {-58, 144}, {-88, 124}, {-103, 105}, {-99, 81}, {-81, 54}, {-56, 36}, {-26, 18}, {6, -0}, {45, -17}, {80, -31}, {113, -46}, {141, -63}, {151, -95}, {151, -112}, {138, -126}, {112, -134}, {84, -137}, {55, -138}, {25, -128}, {-15, -115}, {-52, -105}, {-87, -90}, {-117, -80}, {-149, -70}, {-176, -63}, {-218, -61}, {-256, -61}, {-295, -62}, {-329, -62}, {-377, -65} }, // 索引 0 (上路1)
            { {364, 140},{334, 144}, {299, 147}, {273, 152}, {231, 157}, {195, 160}, {164, 161}, {125, 161}, {87, 160}, {56, 155}, {22, 149}, {-6, 144}, {-32, 136}, {-61, 127}, {-87, 108}, {-87, 79}, {-56, 53}, {-28, 33}, {5, 16}, {29, 4}, {67, -10}, {104, -26}, {142, -43}, {169, -60}, {176, -82}, {175, -103}, {167, -121}, {148, -137}, {122, -145}, {80, -150}, {43, -145}, {7, -137}, {-30, -129}, {-57, -120}, {-93, -106}, {-118, -99}, {-151, -88}, {-182, -77}, {-215, -75}, {-252, -75}, {-293, -78}, {-328, -79}, {-350, -80}, {-371, -81} }, // 索引 1 (上路2)
            { {366, 42},{323, 42}, {299, 44}, {273, 50}, {239, 59}, {214, 65}, {186, 70}, {154, 73}, {121, 75}, {84, 75}, {50, 72}, {25, 62}, {7, 46}, {2, 29}, {0, 11}, {8, -3}, {35, -19}, {58, -28}, {83, -38}, {121, -52}, {144, -66}, {154, -88}, {151, -108}, {140, -123}, {110, -129}, {69, -132}, {33, -126}, {1, -119}, {-29, -106}, {-55, -97}, {-82, -88}, {-111, -79}, {-137, -68}, {-175, -60}, {-210, -57}, {-236, -57}, {-277, -55}, {-315, -62}, {-344, -65}, {-381, -66} },  // 索引 2 (下路1)
            { {366, 8},{353, 9}, {332, 17}, {303, 26}, {256, 35}, {225, 44}, {186, 51}, {155, 53}, {116, 55}, {85, 54}, {58, 46}, {45, 35}, {43, 19}, {46, -0}, {57, -10}, {77, -20}, {112, -30}, {138, -42}, {163, -59}, {177, -84}, {177, -104}, {160, -123}, {134, -139}, {97, -143}, {65, -144}, {27, -144}, {-5, -132}, {-41, -122}, {-74, -106}, {-101, -95}, {-134, -85}, {-172, -74}, {-210, -71}, {-240, -69}, {-283, -71}, {-319, -75}, {-344, -77}, {-369, -78} }   // 索引 3 (下路2)
        };
        config.towerSlotPositions = { {-26, 205}, {-88, 186},{-21, 96},{51, 110},{129, 115},{114, 13},{-81, -39},{-5, -68}, {67, -92},{225, -165},{-153, -139},{-67, -173}};

        // wave1 上路24 Goblin
        WaveConfig w1; float d1 = 0;
        for(int i=0; i<24; i++) { w1.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::GOBLIN}, d1); d1 += 0.5f; }
        config.waves.push_back(w1);

        // wave2 上路5 Orc 5 Goblin
        WaveConfig w2; float d2 = 0;
        for(int i=0; i<5; i++) { w2.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::ORC}, d2); d2 += 1.0f; }
        for(int i=0; i<5; i++) { w2.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::GOBLIN}, d2); d2 += 0.5f; }
        config.waves.push_back(w2);

        // wave3 上路6 Wulf 下路6 wulf
        WaveConfig w3; float d3 = 0;
        for(int i=0; i<6; i++) {
            w3.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::WULF}, d3);
            w3.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::WULF}, d3);
            d3 += 0.5f;
        }
        config.waves.push_back(w3);

        // wave4 上路20 Goblin 下路20 Goblin
        WaveConfig w4; float d4 = 0;
        for(int i=0; i<20; i++) {
            w4.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::GOBLIN}, d4);
            w4.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::GOBLIN}, d4);
            d4 += 0.5f;
        }
        config.waves.push_back(w4);

        // wave5 上路10 Wulf 下路25 Goblin
        WaveConfig w5; float d5_top = 0, d5_bot = 0;
        for(int i=0; i<10; i++) { w5.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::WULF}, d5_top); d5_top += 0.6f; }
        for(int i=0; i<25; i++) { w5.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::GOBLIN}, d5_bot); d5_bot += 0.4f; }
        config.waves.push_back(w5);

        // wave6 上路5 Orc 下路5 Orc
        WaveConfig w6; float d6 = 0;
        for(int i=0; i<5; i++) {
            w6.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::ORC}, d6);
            w6.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::ORC}, d6);
            d6 += 1.2f;
        }
        config.waves.push_back(w6);

        // wave7 上路9 Orc 2 Shaman 下路20 Goblin
        WaveConfig w7; float d7_top = 0, d7_bot = 0;
        for(int i=0; i<9; i++) { w7.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::ORC}, d7_top); d7_top += 1.0f; }
        for(int i=0; i<2; i++) { w7.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::SHAMAN}, d7_top); d7_top += 1.2f; }
        for(int i=0; i<20; i++) { w7.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::GOBLIN}, d7_bot); d7_bot += 0.5f; }
        config.waves.push_back(w7);

        // wave8 上路12 Orc 2 Shaman 下路25 Goblin
        WaveConfig w8; float d8_top = 0, d8_bot = 0;
        for(int i=0; i<12; i++) { w8.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::ORC}, d8_top); d8_top += 1.0f; }
        for(int i=0; i<2; i++) { w8.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::SHAMAN}, d8_top); d8_top += 1.2f; }
        for(int i=0; i<25; i++) { w8.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::GOBLIN}, d8_bot); d8_bot += 0.4f; }
        config.waves.push_back(w8);

        // wave9 上路30 Goblin 下路6 Orc 2 Shaman 15 Goblin 10 Wulf
        WaveConfig w9; float d9_top = 0, d9_bot = 0;
        for(int i=0; i<30; i++) { w9.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::GOBLIN}, d9_top); d9_top += 0.4f; }
        for(int i=0; i<6; i++) { w9.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::ORC}, d9_bot); d9_bot += 1.0f; }
        for(int i=0; i<2; i++) { w9.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::SHAMAN}, d9_bot); d9_bot += 1.2f; }
        for(int i=0; i<15; i++) { w9.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::GOBLIN}, d9_bot); d9_bot += 0.5f; }
        for(int i=0; i<10; i++) { w9.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::WULF}, d9_bot); d9_bot += 0.4f; }
        config.waves.push_back(w9);

        // wave10 1 Ogre 8 Orc 2 Shaman 10 Goblin (注意：若你目前還沒有 OGRE 列舉型態，記得在 Enemy.hpp 補上，這裡先寫為 OGRE 供對齊)
        WaveConfig w10; float d10 = 0;
        w10.subWaves.emplace_back(randomRoute(0, 4), std::vector<Enemy::Type>{Enemy::Type::OGRE}, d10); d10 += 2.0f;
        for(int i=0; i<8; i++) { w10.subWaves.emplace_back(randomRoute(0, 4), std::vector<Enemy::Type>{Enemy::Type::ORC}, d10); d10 += 1.0f; }
        for(int i=0; i<2; i++) { w10.subWaves.emplace_back(randomRoute(0, 4), std::vector<Enemy::Type>{Enemy::Type::SHAMAN}, d10); d10 += 1.2f; }
        for(int i=0; i<10; i++) { w10.subWaves.emplace_back(randomRoute(0, 4), std::vector<Enemy::Type>{Enemy::Type::GOBLIN}, d10); d10 += 0.5f; }
        config.waves.push_back(w10);

        // wave11 1 Ogre 8 Orc 2 Shaman 10 Goblin
        WaveConfig w11; float d11 = 0;
        w11.subWaves.emplace_back(randomRoute(0, 4), std::vector<Enemy::Type>{Enemy::Type::OGRE}, d11); d11 += 2.0f;
        for(int i=0; i<8; i++) { w11.subWaves.emplace_back(randomRoute(0, 4), std::vector<Enemy::Type>{Enemy::Type::ORC}, d11); d11 += 1.0f; }
        for(int i=0; i<2; i++) { w11.subWaves.emplace_back(randomRoute(0, 4), std::vector<Enemy::Type>{Enemy::Type::SHAMAN}, d11); d11 += 1.2f; }
        for(int i=0; i<10; i++) { w11.subWaves.emplace_back(randomRoute(0, 4), std::vector<Enemy::Type>{Enemy::Type::GOBLIN}, d11); d11 += 0.5f; }
        config.waves.push_back(w11);
    }
    // ==================== Level 4 ====================
    // ==================== Level 4 ====================
    else if (levelId == 4) {
        config.initialMoney = 680; // 初始金額改為 680
        config.imagePath = "../PTSD/assets/sprites/images/Map/4.png";
        config.routes = {
            { {-72, -308}, {-68, -294}, {-65, -261}, {-64, -233}, {-65, -205}, {-67, -158}, {-88, -137}, {-117, -115}, {-145, -87}, {-171, -66}, {-198, -41}, {-225, -11}, {-239, 23}, {-243, 48}, {-247, 81}, {-237, 120}, {-220, 147}, {-195, 165}, {-153, 192}, {-138, 200}, {-109, 210}, {-85, 216}, {-50, 222}, {-17, 229}, {6, 232}, {30, 250}, {32, 270}, {33, 293}, {33, 330} }, // 索引 0 (左路1)
            { {-34, -316}, {-36, -307}, {-39, -286}, {-39, -261}, {-38, -242}, {-38, -218}, {-39, -199}, {-41, -177}, {-42, -157}, {-49, -138}, {-67, -122}, {-94, -104}, {-116, -87}, {-133, -68}, {-149, -52}, {-173, -32}, {-188, -17}, {-200, -2}, {-214, 18}, {-226, 44}, {-232, 65}, {-229, 89}, {-221, 117}, {-211, 134}, {-196, 152}, {-176, 168}, {-149, 181}, {-112, 191}, {-82, 201}, {-41, 205}, {5, 209}, {29, 214}, {50, 226}, {61, 246}, {64, 270}, {65, 328} }, // 索引 1 (左路2)
            { {-70, -291}, {-68, -262}, {-65, -240}, {-67, -212}, {-67, -176}, {-60, -144}, {-28, -131}, {17, -130}, {50, -130}, {92, -131}, {138, -125}, {180, -120}, {217, -106}, {248, -75}, {264, -35}, {267, 7}, {253, 45}, {222, 94}, {203, 113}, {181, 131}, {158, 148}, {139, 164}, {123, 176}, {103, 188}, {70, 204}, {48, 218}, {35, 229}, {30, 245}, {32, 281}, {32, 325} },  // 索引 2 (右路1)
            { {-41, -316}, {-44, -289}, {-46, -258}, {-46, -228}, {-48, -201}, {-43, -182}, {-26, -164}, {3, -155}, {37, -151}, {67, -150}, {97, -149}, {133, -148}, {166, -143}, {196, -133}, {224, -121}, {246, -103}, {265, -76}, {273, -52}, {280, -27}, {281, 1}, {275, 26}, {264, 58}, {243, 93}, {223, 111}, {204, 130}, {184, 147}, {165, 162}, {146, 173}, {110, 196}, {84, 207}, {70, 222}, {57, 248},{65, 328} }   // 索引 3 (右路2)
        };
        config.towerSlotPositions = { {-265, -51}, {110, -201},{239, -180}, {-27, -85},{39, -97},{109, -94},{191, -56},{197, 16},{161, 81},{53, 163},{-16, 164},{-86, 152},{-137, 118},{-160, 47},{-137, 1} };

        // ==================== WAVE 1 ====================
        // 左路 10 Goblin 3 Orc | 右路 10 Goblin 3 Orc
        WaveConfig w1; float d1_L = 0.0f, d1_R = 0.0f;
        for (int i = 0; i < 10; ++i) { w1.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::GOBLIN}, d1_L); d1_L += 0.6f; }
        for (int i = 0; i < 3; ++i)  { w1.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::ORC}, d1_L);    d1_L += 1.2f; }
        for (int i = 0; i < 10; ++i) { w1.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::GOBLIN}, d1_R); d1_R += 0.6f; }
        for (int i = 0; i < 3; ++i)  { w1.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::ORC}, d1_R);    d1_R += 1.2f; }
        config.waves.push_back(w1);

        // ==================== WAVE 2 ====================
        // 左路 10 Goblin 4 Orc | 右路 10 Goblin 4 Orc
        WaveConfig w2; float d2_L = 0.0f, d2_R = 0.0f;
        for (int i = 0; i < 10; ++i) { w2.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::GOBLIN}, d2_L); d2_L += 0.6f; }
        for (int i = 0; i < 4; ++i)  { w2.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::ORC}, d2_L);    d2_L += 1.2f; }
        for (int i = 0; i < 10; ++i) { w2.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::GOBLIN}, d2_R); d2_R += 0.6f; }
        for (int i = 0; i < 4; ++i)  { w2.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::ORC}, d2_R);    d2_R += 1.2f; }
        config.waves.push_back(w2);

        // ==================== WAVE 3 ====================
        // 左路 20 Goblin 1 Shaman | 右路 1 Shaman 5 Orc
        WaveConfig w3; float d3_L = 0.0f, d3_R = 0.0f;
        for (int i = 0; i < 20; ++i) { w3.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::GOBLIN}, d3_L); d3_L += 0.5f; }
        w3.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::SHAMAN}, d3_L);
        w3.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::SHAMAN}, d3_R); d3_R += 1.5f;
        for (int i = 0; i < 5; ++i)  { w3.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::ORC}, d3_R);    d3_R += 1.2f; }
        config.waves.push_back(w3);

        // ==================== WAVE 4 ====================
        // 左路 20 Goblin 1 Shaman | 右路 1 Shaman 5 Orc
        WaveConfig w4; float d4_L = 0.0f, d4_R = 0.0f;
        for (int i = 0; i < 20; ++i) { w4.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::GOBLIN}, d4_L); d4_L += 0.5f; }
        w4.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::SHAMAN}, d4_L);
        w4.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::SHAMAN}, d4_R); d4_R += 1.5f;
        for (int i = 0; i < 5; ++i)  { w4.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::ORC}, d4_R);    d4_R += 1.2f; }
        config.waves.push_back(w4);

        // ==================== WAVE 5 ====================
        // 左路 20 Goblin | 右路 2 Shaman 9 Orc
        WaveConfig w5; float d5_L = 0.0f, d5_R = 0.0f;
        for (int i = 0; i < 20; ++i) { w5.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::GOBLIN}, d5_L); d5_L += 0.5f; }
        for (int i = 0; i < 2; ++i)  { w5.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::SHAMAN}, d5_R); d5_R += 1.5f; }
        for (int i = 0; i < 9; ++i)  { w5.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::ORC}, d5_R);    d5_R += 1.0f; }
        config.waves.push_back(w5);

        // ==================== WAVE 6 ====================
        // 左路 3 Bandit | 右路 6 Bandit
        WaveConfig w6; float d6_L = 0.0f, d6_R = 0.0f;
        for (int i = 0; i < 3; ++i)  { w6.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::BANDIT}, d6_L); d6_L += 1.0f; }
        for (int i = 0; i < 6; ++i)  { w6.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::BANDIT}, d6_R); d6_R += 0.8f; }
        config.waves.push_back(w6);

        // ==================== WAVE 7 ====================
        // 左路 9 Wulf 4 Bandit | 右路 9 Wulf 4 Bandit
        WaveConfig w7; float d7_L = 0.0f, d7_R = 0.0f;
        for (int i = 0; i < 9; ++i)  { w7.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::WULF}, d7_L);   d7_L += 0.4f; }
        for (int i = 0; i < 4; ++i)  { w7.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::BANDIT}, d7_L); d7_L += 1.0f; }
        for (int i = 0; i < 9; ++i)  { w7.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::WULF}, d7_R);   d7_R += 0.4f; }
        for (int i = 0; i < 4; ++i)  { w7.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::BANDIT}, d7_R); d7_R += 1.0f; }
        config.waves.push_back(w7);

        // ==================== WAVE 8 ====================
        // 左路 5 Orc 2 Shaman | 右路 1 Ogre 2 Shaman
        WaveConfig w8; float d8_L = 0.0f, d8_R = 0.0f;
        for (int i = 0; i < 5; ++i)  { w8.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::ORC}, d8_L);    d8_L += 1.0f; }
        for (int i = 0; i < 2; ++i)  { w8.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::SHAMAN}, d8_L); d8_L += 1.2f; }
        w8.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::OGRE}, d8_R);  d8_R += 2.0f;
        for (int i = 0; i < 2; ++i)  { w8.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::SHAMAN}, d8_R); d8_R += 1.2f; }
        config.waves.push_back(w8);

        // ==================== WAVE 9 ====================
        // 左路 8 Bandit 7 Orc | 右路 8 Bandit 7 Orc
        WaveConfig w9; float d9_L = 0.0f, d9_R = 0.0f;
        for (int i = 0; i < 8; ++i)  { w9.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::BANDIT}, d9_L); d9_L += 0.8f; }
        for (int i = 0; i < 7; ++i)  { w9.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::ORC}, d9_L);    d9_L += 1.0f; }
        for (int i = 0; i < 8; ++i)  { w9.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::BANDIT}, d9_R); d9_R += 0.8f; }
        for (int i = 0; i < 7; ++i)  { w9.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::ORC}, d9_R);    d9_R += 1.0f; }
        config.waves.push_back(w9);

        // ==================== WAVE 10 ====================
        // 左路 4 Orc 6 Bandit 3 Shaman | 右路 4 Orc 6 Bandit 3 Shaman
        WaveConfig w10; float d10_L = 0.0f, d10_R = 0.0f;
        for (int i = 0; i < 4; ++i)  { w10.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::ORC}, d10_L);    d10_L += 1.0f; }
        for (int i = 0; i < 6; ++i)  { w10.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::BANDIT}, d10_L); d10_L += 0.7f; }
        for (int i = 0; i < 3; ++i)  { w10.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::SHAMAN}, d10_L); d10_L += 1.2f; }
        for (int i = 0; i < 4; ++i)  { w10.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::ORC}, d10_R);    d10_R += 1.0f; }
        for (int i = 0; i < 6; ++i)  { w10.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::BANDIT}, d10_R); d10_R += 0.7f; }
        for (int i = 0; i < 3; ++i)  { w10.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::SHAMAN}, d10_R); d10_R += 1.2f; }
        config.waves.push_back(w10);

        // ==================== WAVE 11 ====================
        // 左路 4 Brigand | 右路 4 Brigand
        WaveConfig w11; float d11_L = 0.0f, d11_R = 0.0f;
        for (int i = 0; i < 4; ++i)  { w11.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::BRIGAND}, d11_L); d11_L += 1.4f; }
        for (int i = 0; i < 4; ++i)  { w11.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::BRIGAND}, d11_R); d11_R += 1.4f; }
        config.waves.push_back(w11);

        // ==================== WAVE 12 ====================
        // 左路 8 Bandit 4 Brigand 3 Shaman | 右路 8 Bandit 4 Brigand 3 Shaman
        WaveConfig w12; float d12_L = 0.0f, d12_R = 0.0f;
        for (int i = 0; i < 8; ++i)  { w12.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::BANDIT}, d12_L);  d12_L += 0.7f; }
        for (int i = 0; i < 4; ++i)  { w12.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::BRIGAND}, d12_L); d12_L += 1.2f; }
        for (int i = 0; i < 3; ++i)  { w12.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::SHAMAN}, d12_L);  d12_L += 1.2f; }
        for (int i = 0; i < 8; ++i)  { w12.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::BANDIT}, d12_R);  d12_R += 0.7f; }
        for (int i = 0; i < 4; ++i)  { w12.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::BRIGAND}, d12_R); d12_R += 1.2f; }
        for (int i = 0; i < 3; ++i)  { w12.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::SHAMAN}, d12_R);  d12_R += 1.2f; }
        config.waves.push_back(w12);

        // ==================== WAVE 13 ====================
        // 左路 6 Bandit 6 Brigand 2 Shaman | 右路 6 Bandit 6 Brigand 2 Shaman
        WaveConfig w13; float d13_L = 0.0f, d13_R = 0.0f;
        for (int i = 0; i < 6; ++i)  { w13.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::BANDIT}, d13_L);  d13_L += 0.7f; }
        for (int i = 0; i < 6; ++i)  { w13.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::BRIGAND}, d13_L); d13_L += 1.1f; }
        for (int i = 0; i < 2; ++i)  { w13.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::SHAMAN}, d13_L);  d13_L += 1.2f; }
        for (int i = 0; i < 6; ++i)  { w13.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::BANDIT}, d13_R);  d13_R += 0.7f; }
        for (int i = 0; i < 6; ++i)  { w13.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::BRIGAND}, d13_R); d13_R += 1.1f; }
        for (int i = 0; i < 2; ++i)  { w13.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::SHAMAN}, d13_R);  d13_R += 1.2f; }
        config.waves.push_back(w13);
    }
    // ==================== Level 5 ====================
    // ==================== Level 5 ====================
    else if (levelId == 5) {
        config.initialMoney = 700; // 初始金額改為 700
        config.imagePath = "../PTSD/assets/sprites/images/Map/5.png";
        config.routes = {
            {{20, -306}, {19, -287}, {22, -262}, {19, -236}, {15, -208}, {6, -176}, {-12, -159}, {-38, -143}, {-63, -135}, {-87, -130}, {-109, -126}, {-138, -119}, {-178, -111}, {-202, -102}, {-228, -83}, {-250, -40}, {-249, -11}, {-242, 20}, {-228, 47}, {-213, 69}, {-196, 84}, {-179, 97}, {-157, 109}, {-135, 120}, {-115, 128}, {-93, 136}, {-63, 143}, {-37, 147}, {-11, 151}, {18, 154}, {37, 156}, {60, 164}, {79, 171}, {99, 184}, {117, 204}, {129, 223}, {134, 243}, {143, 279}, {149, 325}}, // 索引 0 (左路1)
            {{47, -312}, {46, -292}, {43, -268}, {41, -245}, {39, -223}, {36, -201}, {29, -182}, {15, -166}, {2, -153}, {-16, -140}, {-32, -134}, {-63, -121}, {-90, -110}, {-119, -99}, {-154, -90}, {-188, -83}, {-211, -71}, {-223, -45}, {-225, -19}, {-219, 13}, {-211, 34}, {-199, 53}, {-182, 72}, {-161, 88}, {-136, 98}, {-115, 110}, {-92, 118}, {-54, 128}, {-20, 132}, {-4, 134}, {28, 139}, {59, 141}, {78, 145}, {110, 154}, {128, 172}, {142, 194}, {149, 215}, {158, 244}, {164, 265}, {165, 285}, {173, 332}}, // 索引 1 (左路2)
            {{23, -315}, {19, -290}, {24, -272}, {22, -247}, {22, -224}, {17, -205}, {10, -188}, {0, -173}, {-18, -155}, {-37, -143}, {-57, -133}, {-75, -123}, {-91, -118}, {-103, -105}, {-107, -81}, {-105, -54}, {-90, -24}, {-85, -12}, {-70, 4}, {-45, 21}, {-20, 31}, {-1, 38}, {25, 45}, {46, 50}, {65, 54}, {82, 58}, {95, 79}, {95, 96}, {95, 114}, {96, 129}, {103, 159}, {109, 185}, {115, 198}, {128, 220}, {137, 244}, {147, 276}, {149, 326}}, // 索引 2 (中路1)
            {{43, -317}, {45, -302}, {45, -279}, {45, -260}, {43, -235}, {40, -221}, {33, -199}, {27, -178}, {18, -166}, {5, -152}, {-8, -140}, {-18, -135}, {-42, -124}, {-57, -112}, {-68, -88}, {-72, -67}, {-72, -38}, {-65, -21}, {-52, -8}, {-34, 4}, {-12, 11}, {3, 17}, {20, 20}, {42, 24}, {84, 34}, {95, 41}, {109, 58}, {115, 73}, {116, 88}, {116, 101}, {117, 122}, {129, 156}, {138, 179}, {148, 207}, {155, 227}, {164, 260}, {172, 283}, {187, 328}}, // 索引 3 (中路2)
            {{374, -164}, {347, -162}, {331, -161}, {309, -159}, {288, -157}, {261, -156}, {236, -150}, {218, -142}, {205, -128}, {202, -105}, {201, -83}, {218, -59}, {219, -35}, {215, -11}, {208, 4}, {188, 20}, {172, 30}, {147, 46}, {135, 56}, {116, 90}, {116, 116}, {119, 140}, {129, 167}, {138, 193}, {141, 214}, {153, 241}, {164, 271}, {172, 292}, {178, 329}}, // 索引 4 (右路1)
            {{366, -182}, {345, -181}, {324, -174}, {300, -169}, {271, -166}, {247, -165}, {220, -163}, {198, -157}, {184, -143}, {179, -118}, {179, -102}, {184, -86}, {191, -63}, {192, -50}, {192, -30}, {181, -10}, {163, 4}, {132, 18}, {110, 33}, {100, 45}, {90, 70}, {87, 95}, {89, 120}, {91, 140}, {102, 165}, {105, 177}, {115, 203}, {127, 230}, {135, 257}, {143, 282}, {148, 329}}  // 索引 5 (右路2)
        };
        config.towerSlotPositions = { {-119, -178},{-165, -43},{-96, 59},{-238, 124},{38, 198},{172, 104}, {216, 67},{5, -33},{6, -82},{134, -35},{120, -80},{114, -137}};

        // ==================== WAVE 1 ====================
        // 左路 3 Brigand | 中路 4 Bandit
        WaveConfig w1; float d1_L = 0.0f, d1_M = 0.0f;
        for (int i = 0; i < 3; ++i) { w1.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::BRIGAND}, d1_L); d1_L += 1.2f; }
        for (int i = 0; i < 4; ++i) { w1.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::BANDIT}, d1_M);  d1_M += 0.8f; }
        config.waves.push_back(w1);

        // ==================== WAVE 2 ====================
        // 左路 4 Brigand | 中路 8 Bandit
        WaveConfig w2; float d2_L = 0.0f, d2_M = 0.0f;
        for (int i = 0; i < 4; ++i) { w2.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::BRIGAND}, d2_L); d2_L += 1.1f; }
        for (int i = 0; i < 8; ++i) { w2.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::BANDIT}, d2_M);  d2_M += 0.7f; }
        config.waves.push_back(w2);

        // ==================== WAVE 3 ====================
        // 右路 7 Giant-Spider
        WaveConfig w3; float d3_R = 0.0f;
        for (int i = 0; i < 7; ++i) { w3.subWaves.emplace_back(randomRoute(4, 2), std::vector<Enemy::Type>{Enemy::Type::GIANT_SPIDER}, d3_R); d3_R += 0.5f; }
        config.waves.push_back(w3);

        // ==================== WAVE 4 ====================
        // 右路 8 Giant-Spider
        WaveConfig w4; float d4_R = 0.0f;
        for (int i = 0; i < 8; ++i) { w4.subWaves.emplace_back(randomRoute(4, 2), std::vector<Enemy::Type>{Enemy::Type::GIANT_SPIDER}, d4_R); d4_R += 0.5f; }
        config.waves.push_back(w4);

        // ==================== WAVE 5 ====================
        // 左路 5 Brigand 3 Bandit | 中路 3 Brigand 10 Bandit
        WaveConfig w5; float d5_L = 0.0f, d5_M = 0.0f;
        for (int i = 0; i < 5; ++i)  { w5.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::BRIGAND}, d5_L); d5_L += 1.0f; }
        for (int i = 0; i < 3; ++i)  { w5.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::BANDIT}, d5_L);  d5_L += 0.7f; }
        for (int i = 0; i < 3; ++i)  { w5.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::BRIGAND}, d5_M); d5_M += 1.2f; }
        for (int i = 0; i < 10; ++i) { w5.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::BANDIT}, d5_M);  d5_M += 0.6f; }
        config.waves.push_back(w5);

        // ==================== WAVE 6 ====================
        // 左路 5 Brigand 3 Bandit | 中路 3 Brigand 10 Bandit
        WaveConfig w6; float d6_L = 0.0f, d6_M = 0.0f;
        for (int i = 0; i < 5; ++i)  { w6.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::BRIGAND}, d6_L); d6_L += 1.0f; }
        for (int i = 0; i < 3; ++i)  { w6.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::BANDIT}, d6_L);  d6_L += 0.7f; }
        for (int i = 0; i < 3; ++i)  { w6.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::BRIGAND}, d6_M); d6_M += 1.2f; }
        for (int i = 0; i < 10; ++i) { w6.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::BANDIT}, d6_M);  d6_M += 0.6f; }
        config.waves.push_back(w6);

        // ==================== WAVE 7 ====================
        // 左路 15 Wulf | 中路 15 Wulf
        WaveConfig w7; float d7_L = 0.0f, d7_M = 0.0f;
        for (int i = 0; i < 15; ++i) { w7.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::WULF}, d7_L); d7_L += 0.4f; }
        for (int i = 0; i < 15; ++i) { w7.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::WULF}, d7_M); d7_M += 0.4f; }
        config.waves.push_back(w7);

        // ==================== WAVE 8 ====================
        // 左路 3 Worg | 中路 3 Worg
        WaveConfig w8; float d8_L = 0.0f, d8_M = 0.0f;
        for (int i = 0; i < 3; ++i) { w8.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::WORG}, d8_L); d8_L += 1.0f; }
        for (int i = 0; i < 3; ++i) { w8.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::WORG}, d8_M); d8_M += 1.0f; }
        config.waves.push_back(w8);

        // ==================== WAVE 9 ====================
        // 左路 6 Worg 5 Wulf | 中路 6 Worg 5 Wulf
        WaveConfig w9; float d9_L = 0.0f, d9_M = 0.0f;
        for (int i = 0; i < 6; ++i) { w9.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::WORG}, d9_L); d9_L += 0.9f; }
        for (int i = 0; i < 5; ++i) { w9.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::WULF}, d9_L); d9_L += 0.4f; }
        for (int i = 0; i < 6; ++i) { w9.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::WORG}, d9_M); d9_M += 0.9f; }
        for (int i = 0; i < 5; ++i) { w9.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::WULF}, d9_M); d9_M += 0.4f; }
        config.waves.push_back(w9);

        // ==================== WAVE 10 ====================
        // 右路 20 Giant-Spider
        WaveConfig w10; float d10_R = 0.0f;
        for (int i = 0; i < 20; ++i) { w10.subWaves.emplace_back(randomRoute(4, 2), std::vector<Enemy::Type>{Enemy::Type::GIANT_SPIDER}, d10_R); d10_R += 0.4f; }
        config.waves.push_back(w10);

        // ==================== WAVE 11 ====================
        // 右路 2 Spider-Matriarch
        WaveConfig w11; float d11_R = 0.0f;
        for (int i = 0; i < 20; ++i) { w11.subWaves.emplace_back(randomRoute(4, 2), std::vector<Enemy::Type>{Enemy::Type::SPIDER_MATRIARCH}, d11_R); d11_R += 4.0f; }
        config.waves.push_back(w11);

        // ==================== WAVE 12 ====================
        // 右路 3 Spider-Matriarch
        WaveConfig w12; float d12_R = 0.0f;
        for (int i = 0; i < 3; ++i) { w12.subWaves.emplace_back(randomRoute(4, 2), std::vector<Enemy::Type>{Enemy::Type::SPIDER_MATRIARCH}, d12_R); d12_R += 3.5f; }
        config.waves.push_back(w12);

        // ==================== WAVE 13 ====================
        // 左路 6 Worg | 中路 15 Wulf | 右路 4 Spider-Matriarch
        WaveConfig w13; float d13_L = 0.0f, d13_M = 0.0f, d13_R = 0.0f;
        for (int i = 0; i < 6; ++i)  { w13.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::WORG}, d13_L);            d13_L += 0.8f; }
        for (int i = 0; i < 15; ++i) { w13.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::WULF}, d13_M);            d13_M += 0.4f; }
        for (int i = 0; i < 4; ++i)  { w13.subWaves.emplace_back(randomRoute(4, 2), std::vector<Enemy::Type>{Enemy::Type::SPIDER_MATRIARCH}, d13_R); d13_R += 3.5f; }
        config.waves.push_back(w13);

        // ==================== WAVE 14 ====================
        // 左路 12 Brigand | 中路 12 Bandit 8 Worg | 右路 4 Spider-Matriarch
        WaveConfig w14; float d14_L = 0.0f, d14_M = 0.0f, d14_R = 0.0f;
        for (int i = 0; i < 12; ++i) { w14.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::BRIGAND}, d14_L);           d14_L += 0.8f; }
        for (int i = 0; i < 12; ++i) { w14.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::BANDIT}, d14_M);            d14_M += 0.5f; }
        for (int i = 0; i < 8; ++i)  { w14.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::WORG}, d14_M);              d14_M += 0.8f; }
        for (int i = 0; i < 4; ++i)  { w14.subWaves.emplace_back(randomRoute(4, 2), std::vector<Enemy::Type>{Enemy::Type::SPIDER_MATRIARCH}, d14_R); d14_R += 3.0f; }
        config.waves.push_back(w14);

        // ==================== WAVE 15 ====================
        // 左路 1 Marauder 3 Brigand 6 Bandit | 中路 1 Marauder 3 Brigand 6 Bandit
        WaveConfig w15; float d15_L = 0.0f, d15_M = 0.0f;
        // 左路隊伍
        w15.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::MARAUDER}, d15_L); d15_L += 2.0f;
        for (int i = 0; i < 3; ++i)  { w15.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::BRIGAND}, d15_L); d15_L += 0.9f; }
        for (int i = 0; i < 6; ++i)  { w15.subWaves.emplace_back(randomRoute(0, 2), std::vector<Enemy::Type>{Enemy::Type::BANDIT}, d15_L);  d15_L += 0.6f; }
        // 中路隊伍
        w15.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::MARAUDER}, d15_M); d15_M += 2.0f;
        for (int i = 0; i < 3; ++i)  { w15.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::BRIGAND}, d15_M); d15_M += 0.9f; }
        for (int i = 0; i < 6; ++i)  { w15.subWaves.emplace_back(randomRoute(2, 2), std::vector<Enemy::Type>{Enemy::Type::BANDIT}, d15_M);  d15_M += 0.6f; }
        config.waves.push_back(w15);
    }

    return std::make_shared<Map>(config);
}