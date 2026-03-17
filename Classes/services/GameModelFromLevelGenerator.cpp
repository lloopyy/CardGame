#include "services/GameModelFromLevelGenerator.h"

#include "configs/models/GameLayoutConfig.h"

USING_NS_CC;

GameModel GameModelFromLevelGenerator::generate(const LevelConfig& levelConfig)
{
    GameModel model;

    for (size_t i = 0; i < levelConfig.playField.size(); ++i)
    {
        CardModel card;
        card.face = levelConfig.playField[i].cardFace;
        card.suit = levelConfig.playField[i].cardSuit;
        card.position = levelConfig.playField[i].position;
        card.isInPlayField = true;
        card.renderOrder = static_cast<int>(i);
        const int id = model.addCard(card);
        model.playFieldCardIds.push_back(id);
    }

    for (size_t i = 0; i < levelConfig.stack.size(); ++i)
    {
        const size_t configIndex = levelConfig.stack.size() - 1 - i;
        CardModel card;
        card.face = levelConfig.stack[configIndex].cardFace;
        card.suit = levelConfig.stack[configIndex].cardSuit;
        card.position = levelConfig.stack[configIndex].position;
        card.isInPlayField = false;
        card.renderOrder = static_cast<int>(i);
        const int id = model.addCard(card);
        model.stackCardIds.push_back(id);
    }

    if (!model.stackCardIds.empty())
    {
        model.topCardId = model.stackCardIds.front();
        model.stackCardIds.erase(model.stackCardIds.begin());

        CardModel* topCard = model.getCard(model.topCardId);
        if (topCard != nullptr)
        {
            topCard->position = GameLayoutConfig::getTopCardAnchorPosition();
            topCard->isInPlayField = false;
        }
    }

    return model;
}
