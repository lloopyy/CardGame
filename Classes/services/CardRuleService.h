#ifndef __CARD_RULE_SERVICE_H__
#define __CARD_RULE_SERVICE_H__

#include "models/CardTypes.h"
#include <cstdlib>

/**
 * @brief 无状态卡牌规则服务。
 */
class CardRuleService
{
public:
    /**
     * @brief 判断两张牌是否可匹配（点数差 1，无花色限制）。
     */
    static bool canMatch(CardFaceType currentTop, CardFaceType candidate)
    {
        if (currentTop == CFT_NONE || candidate == CFT_NONE)
        {
            return false;
        }

        const int delta = std::abs(static_cast<int>(currentTop) - static_cast<int>(candidate));
        return delta == 1;
    }
};

#endif // __CARD_RULE_SERVICE_H__
