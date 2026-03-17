#ifndef __UNDO_MODEL_H__
#define __UNDO_MODEL_H__

#include "cocos2d.h"
#include <vector>

/**
 * @brief 回退动作类型。
 */
enum class UndoActionType
{
    ReplaceTopCard
};

/**
 * @brief 单次回退记录数据。
 */
struct UndoRecord
{
    UndoActionType actionType = UndoActionType::ReplaceTopCard;
    int movedCardId = -1;
    int previousTopCardId = -1;
    cocos2d::Vec2 movedFrom = cocos2d::Vec2::ZERO;
    cocos2d::Vec2 movedTo = cocos2d::Vec2::ZERO;
    bool movedFromPlayField = false;

    cocos2d::ValueMap toValueMap() const
    {
        cocos2d::ValueMap map;
        map["actionType"] = static_cast<int>(actionType);
        map["movedCardId"] = movedCardId;
        map["previousTopCardId"] = previousTopCardId;
        map["movedFromX"] = movedFrom.x;
        map["movedFromY"] = movedFrom.y;
        map["movedToX"] = movedTo.x;
        map["movedToY"] = movedTo.y;
        map["movedFromPlayField"] = movedFromPlayField;
        return map;
    }

    static UndoRecord fromValueMap(const cocos2d::ValueMap& map)
    {
        UndoRecord record;

        auto iter = map.find("actionType");
        if (iter != map.end())
        {
            record.actionType = static_cast<UndoActionType>(iter->second.asInt());
        }

        iter = map.find("movedCardId");
        if (iter != map.end())
        {
            record.movedCardId = iter->second.asInt();
        }

        iter = map.find("previousTopCardId");
        if (iter != map.end())
        {
            record.previousTopCardId = iter->second.asInt();
        }

        float movedFromX = 0.0f;
        float movedFromY = 0.0f;
        float movedToX = 0.0f;
        float movedToY = 0.0f;

        iter = map.find("movedFromX");
        if (iter != map.end())
        {
            movedFromX = iter->second.asFloat();
        }

        iter = map.find("movedFromY");
        if (iter != map.end())
        {
            movedFromY = iter->second.asFloat();
        }

        iter = map.find("movedToX");
        if (iter != map.end())
        {
            movedToX = iter->second.asFloat();
        }

        iter = map.find("movedToY");
        if (iter != map.end())
        {
            movedToY = iter->second.asFloat();
        }

        iter = map.find("movedFromPlayField");
        if (iter != map.end())
        {
            record.movedFromPlayField = iter->second.asBool();
        }

        record.movedFrom = cocos2d::Vec2(movedFromX, movedFromY);
        record.movedTo = cocos2d::Vec2(movedToX, movedToY);
        return record;
    }
};

/**
 * @brief 撤销记录模型。
 */
class UndoModel
{
public:
    /** @brief 回退记录栈，尾部为最近一次动作。 */
    std::vector<UndoRecord> records;

    /**
     * @brief 清空所有回退记录。
     */
    void clear()
    {
        records.clear();
    }

    /**
     * @brief 序列化回退模型。
     */
    cocos2d::ValueMap toValueMap() const
    {
        cocos2d::ValueVector recordArray;
        for (const UndoRecord& record : records)
        {
            recordArray.push_back(cocos2d::Value(record.toValueMap()));
        }

        cocos2d::ValueMap map;
        map["records"] = cocos2d::Value(recordArray);
        return map;
    }

    /**
     * @brief 从 ValueMap 反序列化回退模型。
     */
    static UndoModel fromValueMap(const cocos2d::ValueMap& map)
    {
        UndoModel model;
        auto iter = map.find("records");
        if (iter == map.end())
        {
            return model;
        }

        const cocos2d::ValueVector& recordArray = iter->second.asValueVector();
        model.records.reserve(recordArray.size());
        for (const cocos2d::Value& value : recordArray)
        {
            model.records.push_back(UndoRecord::fromValueMap(value.asValueMap()));
        }
        return model;
    }
};

#endif // __UNDO_MODEL_H__
