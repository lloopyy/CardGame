#ifndef __LEVEL_CONFIG_LOADER_H__
#define __LEVEL_CONFIG_LOADER_H__

#include "configs/models/LevelConfig.h"
#include <string>

/**
 * @brief 关卡配置加载器。
 */
class LevelConfigLoader
{
public:
    /**
     * @brief 从 JSON 文件加载关卡配置。
     * @param filePath 相对 Resources 的路径。
     * @param outConfig 加载成功后输出配置。
     * @return 是否加载成功。
     */
    static bool loadFromFile(const std::string& filePath, LevelConfig& outConfig);
};

#endif // __LEVEL_CONFIG_LOADER_H__
