#include "configs/loaders/LevelConfigLoader.h"

#include "json/document.h"

using namespace cocos2d;

namespace
{
    bool getRequiredInt(const rapidjson::Value& obj, const char* key, int& outValue)
    {
        if (!obj.IsObject() || !obj.HasMember(key) || !obj[key].IsInt())
        {
            return false;
        }

        outValue = obj[key].GetInt();
        return true;
    }

    bool getRequiredFloat(const rapidjson::Value& obj, const char* key, float& outValue)
    {
        if (!obj.IsObject() || !obj.HasMember(key) || !obj[key].IsNumber())
        {
            return false;
        }

        outValue = obj[key].GetFloat();
        return true;
    }

    bool parseCards(const rapidjson::Value& cards, std::vector<LevelCardConfig>& outCards)
    {
        if (!cards.IsArray())
        {
            return false;
        }

        for (rapidjson::SizeType i = 0; i < cards.Size(); ++i)
        {
            const rapidjson::Value& item = cards[i];
            if (!item.IsObject())
            {
                return false;
            }

            if (!item.HasMember("Position") || !item["Position"].IsObject())
            {
                return false;
            }

            int face = static_cast<int>(CFT_NONE);
            int suit = static_cast<int>(CST_NONE);
            if (!getRequiredInt(item, "CardFace", face) || !getRequiredInt(item, "CardSuit", suit))
            {
                return false;
            }

            if (face < static_cast<int>(CFT_ACE) || face >= static_cast<int>(CFT_NUM_CARD_FACE_TYPES))
            {
                return false;
            }

            if (suit < static_cast<int>(CST_CLUBS) || suit >= static_cast<int>(CST_NUM_CARD_SUIT_TYPES))
            {
                return false;
            }

            const rapidjson::Value& pos = item["Position"];
            float x = 0.0f;
            float y = 0.0f;
            if (!getRequiredFloat(pos, "x", x) || !getRequiredFloat(pos, "y", y))
            {
                return false;
            }

            LevelCardConfig cfg;
            cfg.cardFace = static_cast<CardFaceType>(face);
            cfg.cardSuit = static_cast<CardSuitType>(suit);
            cfg.position = Vec2(x, y);
            outCards.push_back(cfg);
        }

        return true;
    }
}

bool LevelConfigLoader::loadFromFile(const std::string& filePath, LevelConfig& outConfig)
{
    std::string text = FileUtils::getInstance()->getStringFromFile(filePath);
    if (text.empty())
    {
        CCLOG("LevelConfigLoader: file empty or missing: %s", filePath.c_str());
        return false;
    }

    rapidjson::Document doc;
    doc.Parse<0>(text.c_str());
    if (doc.HasParseError() || !doc.IsObject())
    {
        CCLOG("LevelConfigLoader: parse failed: %s", filePath.c_str());
        return false;
    }

    if (!doc.HasMember("Playfield") || !doc.HasMember("Stack"))
    {
        CCLOG("LevelConfigLoader: invalid json structure: %s", filePath.c_str());
        return false;
    }

    outConfig.playField.clear();
    outConfig.stack.clear();

    if (!parseCards(doc["Playfield"], outConfig.playField))
    {
        CCLOG("LevelConfigLoader: Playfield parse failed: %s", filePath.c_str());
        return false;
    }

    if (!parseCards(doc["Stack"], outConfig.stack))
    {
        CCLOG("LevelConfigLoader: Stack parse failed: %s", filePath.c_str());
        return false;
    }

    return true;
}
