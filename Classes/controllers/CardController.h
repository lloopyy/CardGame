#ifndef __CARD_CONTROLLER_H__
#define __CARD_CONTROLLER_H__

#include "configs/models/GameLayoutConfig.h"
#include "models/GameModel.h"

/**
 * @brief 卡牌逻辑控制器，封装卡牌选择的基础判断。
 */
class CardController
{
public:
    /**
     * @brief 判断指定卡牌是否可作为主牌区可点击目标。
     */
    static bool canSelectPlayFieldCard(const GameModel& model, int cardId)
    {
        const CardModel* card = model.getCard(cardId);
        if (card == nullptr || !card->isInPlayField || !card->isFaceUp)
        {
            return false;
        }

        const cocos2d::Rect cardRect(card->position.x,
                                     card->position.y,
                                     GameLayoutConfig::kCardWidth,
                                     GameLayoutConfig::kCardHeight);

        for (int playFieldCardId : model.playFieldCardIds)
        {
            if (playFieldCardId == cardId)
            {
                continue;
            }

            const CardModel* other = model.getCard(playFieldCardId);
            if (other == nullptr || !other->isInPlayField)
            {
                continue;
            }

            if (other->renderOrder <= card->renderOrder)
            {
                continue;
            }

            const cocos2d::Rect otherRect(other->position.x,
                                          other->position.y,
                                          GameLayoutConfig::kCardWidth,
                                          GameLayoutConfig::kCardHeight);
            if (cardRect.intersectsRect(otherRect))
            {
                return false;
            }
        }

        return true;
    }
};

#endif // __CARD_CONTROLLER_H__
