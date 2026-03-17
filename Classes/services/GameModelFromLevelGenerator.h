#ifndef __GAME_MODEL_FROM_LEVEL_GENERATOR_H__
#define __GAME_MODEL_FROM_LEVEL_GENERATOR_H__

#include "configs/models/LevelConfig.h"
#include "models/GameModel.h"

/**
 * @brief 将静态关卡配置转换为运行时模型。
 */
class GameModelFromLevelGenerator
{
public:
    /**
     * @brief 根据关卡配置生成游戏运行时模型。
     */
    static GameModel generate(const LevelConfig& levelConfig);
};

#endif // __GAME_MODEL_FROM_LEVEL_GENERATOR_H__
