#ifndef MAP_FACTORY_HPP
#define MAP_FACTORY_HPP

#include <memory>
#include "Map.hpp"

class MapFactory {
public:
    static std::shared_ptr<Map> CreateLevel(int levelId);

private:
    MapFactory() = delete; // 靜態工廠不需要實例化
};

#endif