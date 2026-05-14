cmake_minimum_required(VERSION 3.16)
project(REPLACE_WITH_YOUR_PROJECT_NAME)

set(CMAKE_CXX_STANDARD 17)

cmake_policy(SET CMP0135 NEW)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
        ptsd
        URL         https://github.com/ntut-open-source-club/practical-tools-for-simple-design/archive/refs/tags/v0.2.zip
        SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/PTSD
)

FetchContent_MakeAvailable(ptsd)

include(files.cmake)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
list(TRANSFORM SRC_FILES PREPEND ${SRC_DIR}/)
list(TRANSFORM INCLUDE_FILES PREPEND ${INCLUDE_DIR}/)

add_executable(${PROJECT_NAME}
        ${SRC_DIR}/main.cpp
        ${SRC_FILES}
        ${HEADER_FILES}
        include/Map.hpp
        include/MapManager.hpp
        include/TowerSlot.hpp
        include/MapFactory.hpp
        include/Unit.hpp
        include/EnemyFactory.hpp
        include/Enemy.hpp
        include/ControlStage.hpp
        include/TowerManager.hpp
        include/Tower.hpp
        include/WorldMap.hpp
        include/BuildMenu.hpp
        include/GameManager.hpp
        include/Hud.hpp
        include/HudValue.hpp
        include/VictoryMenu.hpp
        include/MapData.hpp

        # 修正後的 Projectile 分類路徑
        include/Tower/Projectile/Projectile.hpp
        include/Tower/Projectile/Bomb.hpp
        include/Tower/Projectile/MagicBolt.hpp
        include/Tower/Projectile/Arrow.hpp

        # Tower 目錄下的 Header
        include/Tower/ArcherTower.h
        include/Tower/MageTower.h
        include/Tower/BombTower.h
        include/Tower/Barracks.h

        # Source 檔案
        src/Tower/ArcherTower.cpp
        src/Tower/MageTower.cpp
        src/Tower/BombTower.cpp
        src/Tower/Barracks.cpp
        src/Soldier.cpp
        src/Hud.cpp
        src/Enemy.cpp
        src/VictoryMenu.cpp
        src/MapFactory.cpp
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/Resources")
else()
    message(FATAL_ERROR "relative RESOURCE_DIR is WIP, Please use `-DCMAKE_BUILD_TYPE=Debug` build for now.")
endif()

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${DEPENDENCY_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/PTSD/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 讓你在程式碼中可以直接寫 #include "Projectile.hpp" 而不用寫長路徑
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/Tower/Projectile)

target_link_libraries(${PROJECT_NAME}
        SDL2::SDL2main
        PTSD
)