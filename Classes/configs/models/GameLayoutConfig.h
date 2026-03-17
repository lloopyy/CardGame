#ifndef __GAME_LAYOUT_CONFIG_H__
#define __GAME_LAYOUT_CONFIG_H__

#include "cocos2d.h"

/**
 * @brief 游戏布局静态配置。
 */
class GameLayoutConfig
{
public:
    static constexpr float kDesignWidth = 1080.0f;
    static constexpr float kDesignHeight = 2080.0f;

    static constexpr float kPlayFieldWidth = 1080.0f;
    static constexpr float kPlayFieldHeight = 1500.0f;

    static constexpr float kStackWidth = 1080.0f;
    static constexpr float kStackHeight = 580.0f;

    static constexpr float kCardWidth = 144.0f;
    // Keep card_general.png (182x282) aspect ratio to avoid stretched look.
    static constexpr float kCardHeight = 223.2f;

    static cocos2d::Rect getPlayFieldRect()
    {
        return cocos2d::Rect(0.0f, kStackHeight, kPlayFieldWidth, kPlayFieldHeight);
    }

    static cocos2d::Rect getStackRect()
    {
        return cocos2d::Rect(0.0f, 0.0f, kStackWidth, kStackHeight);
    }

    static cocos2d::Vec2 getTopCardAnchorPosition()
    {
        return cocos2d::Vec2(700.0f, 280.0f);
    }

    static cocos2d::Vec2 getDrawStackAnchorPosition()
    {
        return cocos2d::Vec2(200.0f, 280.0f);
    }
};

#endif // __GAME_LAYOUT_CONFIG_H__