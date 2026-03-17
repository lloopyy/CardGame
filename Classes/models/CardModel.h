#ifndef __CARD_MODEL_H__
#define __CARD_MODEL_H__

#include "cocos2d.h"
#include "models/CardTypes.h"

/**
 * @brief 单张卡牌运行时数据。
 *
 * models 层只保存状态，不处理业务流程。
 */
class CardModel
{
public:
    int cardId = -1;
    CardFaceType face = CFT_NONE;
    CardSuitType suit = CST_NONE;
    cocos2d::Vec2 position = cocos2d::Vec2::ZERO;
    bool isInPlayField = false;
    bool isFaceUp = true;
    int renderOrder = 0;

    /**
     * @brief 序列化为 ValueMap，便于存档。
     */
    cocos2d::ValueMap toValueMap() const
    {
        cocos2d::ValueMap map;
        map["cardId"] = cardId;
        map["face"] = static_cast<int>(face);
        map["suit"] = static_cast<int>(suit);
        map["x"] = position.x;
        map["y"] = position.y;
        map["isInPlayField"] = isInPlayField;
        map["isFaceUp"] = isFaceUp;
        map["renderOrder"] = renderOrder;
        return map;
    }

    /**
     * @brief 从 ValueMap 反序列化。
     */
    static CardModel fromValueMap(const cocos2d::ValueMap& map)
    {
        CardModel model;
        model.cardId = map.at("cardId").asInt();
        model.face = static_cast<CardFaceType>(map.at("face").asInt());
        model.suit = static_cast<CardSuitType>(map.at("suit").asInt());
        model.position = cocos2d::Vec2(map.at("x").asFloat(), map.at("y").asFloat());
        model.isInPlayField = map.at("isInPlayField").asBool();
        model.isFaceUp = map.at("isFaceUp").asBool();
        if (map.find("renderOrder") != map.end())
        {
            model.renderOrder = map.at("renderOrder").asInt();
        }
        return model;
    }
};

#endif // __CARD_MODEL_H__
