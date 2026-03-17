#ifndef __CARD_UTILS_H__
#define __CARD_UTILS_H__

#include "models/CardTypes.h"
#include <string>

/**
 * @brief 卡牌展示文案工具。
 */
class CardUtils
{
public:
    static std::string faceToString(CardFaceType face)
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
        default: return "?";
        }
    }

    static std::string suitToString(CardSuitType suit)
    {
        switch (suit)
        {
        case CST_CLUBS: return "C";
        case CST_DIAMONDS: return "D";
        case CST_HEARTS: return "H";
        case CST_SPADES: return "S";
        default: return "N";
        }
    }

    static std::string toCardName(CardSuitType suit, CardFaceType face)
    {
        return suitToString(suit) + faceToString(face);
    }
};

#endif // __CARD_UTILS_H__
