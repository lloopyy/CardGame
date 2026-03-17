#ifndef __GAME_MODEL_H__
#define __GAME_MODEL_H__

#include "models/CardModel.h"
#include "models/UndoModel.h"
#include <unordered_map>
#include <vector>

/**
 * @brief 游戏运行时总模型。
 */
class GameModel
{
public:
    /** @brief 所有卡牌的运行时数据，键为 cardId。 */
    std::unordered_map<int, CardModel> cards;
    /** @brief 主牌区卡牌 ID 列表。 */
    std::vector<int> playFieldCardIds;
    /** @brief 备用牌堆卡牌 ID 列表，front 为当前可抽取顶部。 */
    std::vector<int> stackCardIds;
    /** @brief 当前手牌区顶部卡牌 ID。 */
    int topCardId = -1;
    /** @brief 新卡牌分配 ID 的自增游标。 */
    int nextCardId = 1;
    /** @brief 回退记录模型。 */
    UndoModel undoModel;

    /**
     * @brief 添加卡牌并分配唯一 ID。
     */
    int addCard(const CardModel& card)
    {
        CardModel copy = card;
        copy.cardId = nextCardId++;
        cards[copy.cardId] = copy;
        return copy.cardId;
    }

    /**
     * @brief 按 ID 获取卡牌模型（可写）。
     */
    CardModel* getCard(int cardId)
    {
        auto iter = cards.find(cardId);
        if (iter == cards.end())
        {
            return nullptr;
        }
        return &(iter->second);
    }

    /**
     * @brief 按 ID 获取卡牌模型（只读）。
     */
    const CardModel* getCard(int cardId) const
    {
        auto iter = cards.find(cardId);
        if (iter == cards.end())
        {
            return nullptr;
        }
        return &(iter->second);
    }

    /**
     * @brief 序列化游戏模型，便于存档。
     */
    cocos2d::ValueMap toValueMap() const
    {
        cocos2d::ValueVector cardsArray;
        cardsArray.reserve(cards.size());
        for (const auto& pair : cards)
        {
            cardsArray.push_back(cocos2d::Value(pair.second.toValueMap()));
        }

        cocos2d::ValueVector playFieldArray;
        playFieldArray.reserve(playFieldCardIds.size());
        for (int cardId : playFieldCardIds)
        {
            playFieldArray.push_back(cocos2d::Value(cardId));
        }

        cocos2d::ValueVector stackArray;
        stackArray.reserve(stackCardIds.size());
        for (int cardId : stackCardIds)
        {
            stackArray.push_back(cocos2d::Value(cardId));
        }

        cocos2d::ValueMap map;
        map["cards"] = cocos2d::Value(cardsArray);
        map["playFieldCardIds"] = cocos2d::Value(playFieldArray);
        map["stackCardIds"] = cocos2d::Value(stackArray);
        map["topCardId"] = topCardId;
        map["nextCardId"] = nextCardId;
        map["undoModel"] = cocos2d::Value(undoModel.toValueMap());
        return map;
    }

    /**
     * @brief 从 ValueMap 反序列化游戏模型。
     */
    static GameModel fromValueMap(const cocos2d::ValueMap& map)
    {
        GameModel model;

        auto iter = map.find("cards");
        if (iter != map.end())
        {
            const cocos2d::ValueVector& cardsArray = iter->second.asValueVector();
            for (const cocos2d::Value& value : cardsArray)
            {
                CardModel card = CardModel::fromValueMap(value.asValueMap());
                model.cards[card.cardId] = card;
            }
        }

        iter = map.find("playFieldCardIds");
        if (iter != map.end())
        {
            const cocos2d::ValueVector& playFieldArray = iter->second.asValueVector();
            model.playFieldCardIds.reserve(playFieldArray.size());
            for (const cocos2d::Value& value : playFieldArray)
            {
                model.playFieldCardIds.push_back(value.asInt());
            }
        }

        iter = map.find("stackCardIds");
        if (iter != map.end())
        {
            const cocos2d::ValueVector& stackArray = iter->second.asValueVector();
            model.stackCardIds.reserve(stackArray.size());
            for (const cocos2d::Value& value : stackArray)
            {
                model.stackCardIds.push_back(value.asInt());
            }
        }

        iter = map.find("topCardId");
        if (iter != map.end())
        {
            model.topCardId = iter->second.asInt();
        }

        iter = map.find("nextCardId");
        if (iter != map.end())
        {
            model.nextCardId = iter->second.asInt();
        }

        iter = map.find("undoModel");
        if (iter != map.end())
        {
            model.undoModel = UndoModel::fromValueMap(iter->second.asValueMap());
        }

        return model;
    }
};

#endif // __GAME_MODEL_H__
