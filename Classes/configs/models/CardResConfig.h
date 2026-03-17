#ifndef __CARD_RES_CONFIG_H__
#define __CARD_RES_CONFIG_H__

#include "models/CardTypes.h"
#include <string>

/**
 * @brief 卡牌资源路径规则配置。
 */
class CardResConfig
{
public:
    static std::string getCardFrontPath()
    {
        return "res/card_general.png";
    }

    /**
     * @brief 返回卡牌贴图路径。
     */
    static std::string getCardSpritePath(CardSuitType suit, CardFaceType face)
    {
        (void)suit;
        (void)face;
        return getCardFrontPath();
    }

    static std::string getSuitIconPath(CardSuitType suit)
    {
        switch (suit)
        {
        case CST_CLUBS: return "res/suits/club.png";
        case CST_DIAMONDS: return "res/suits/diamond.png";
        case CST_HEARTS: return "res/suits/heart.png";
        case CST_SPADES: return "res/suits/spade.png";
        default: return "";
        }
    }

    static std::string getSmallNumberPath(CardSuitType suit, CardFaceType face)
    {
        const std::string color = isRedSuit(suit) ? "red" : "black";
        const std::string faceToken = faceToToken(face);
        if (faceToken.empty())
        {
            return "";
        }

        return "res/number/small_" + color + "_" + faceToken + ".png";
    }

    static std::string getBigNumberPath(CardSuitType suit, CardFaceType face)
    {
        const std::string color = isRedSuit(suit) ? "red" : "black";
        const std::string faceToken = faceToToken(face);
        if (faceToken.empty())
        {
            return "";
        }

        return "res/number/big_" + color + "_" + faceToken + ".png";
    }

    /**
     * @brief 返回卡背贴图路径。
     */
    static std::string getCardBackPath()
    {
        return "res/card_general.png";
    }

private:
    static bool isRedSuit(CardSuitType suit)
    {
        return suit == CST_DIAMONDS || suit == CST_HEARTS;
    }

    static std::string faceToToken(CardFaceType face)
    {
        switch (face)
        {
        case CFT_ACE: return "A";
        case CFT_TWO: return "2";
        case CFT_THREE: return "3";
        case CFT_FOUR: return "4";
        case CFT_FIVE: return "5";
        case CFT_SIX: return "6";
        case CFT_SEVEN: return "7";
        case CFT_EIGHT: return "8";
        case CFT_NINE: return "9";
        case CFT_TEN: return "10";
        case CFT_JACK: return "J";
        case CFT_QUEEN: return "Q";
        case CFT_KING: return "K";
        default: return "";
        }
    }
};

#endif // __CARD_RES_CONFIG_H__
