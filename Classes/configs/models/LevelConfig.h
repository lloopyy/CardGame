#ifndef __LEVEL_CONFIG_H__
#define __LEVEL_CONFIG_H__

#include "cocos2d.h"
#include "models/CardTypes.h"
#include <vector>

/**
 * @brief 关卡中单张卡牌的静态配置。
 */
struct LevelCardConfig
{
    CardFaceType cardFace = CFT_NONE;
    CardSuitType cardSuit = CST_NONE;
    cocos2d::Vec2 position = cocos2d::Vec2::ZERO;
};

/**
 * @brief 关卡静态配置数据。
 */
class LevelConfig
{
public:
    std::vector<LevelCardConfig> playField;
    std::vector<LevelCardConfig> stack;
};

#endif // __LEVEL_CONFIG_H__
