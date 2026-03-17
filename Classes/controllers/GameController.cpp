#include "controllers/GameController.h"

#include "configs/loaders/LevelConfigLoader.h"
#include "configs/models/GameLayoutConfig.h"
#include "controllers/CardController.h"
#include "services/CardRuleService.h"
#include "services/GameModelFromLevelGenerator.h"
#include "views/GameView.h"
#include <algorithm>
#include <vector>

USING_NS_CC;

namespace
{
constexpr float kStackY = 280.0f;
constexpr float kStackOffsetX = 40.0f;
const std::vector<std::string> kLevelPaths = {
    "res/levels/level_1.json",
    "res/levels/level_2.json"
};

int findLevelIndex(const std::string& levelPath)
{
    for (size_t i = 0; i < kLevelPaths.size(); ++i)
    {
        if (kLevelPaths[i] == levelPath)
        {
            return static_cast<int>(i);
        }
    }

    return -1;
}

float getStackStartX()
{
    const Vec2 drawAnchor = GameLayoutConfig::getDrawStackAnchorPosition();
    const float deckCenterX = drawAnchor.x + 24.0f + (GameLayoutConfig::kCardWidth * 0.5f);
    return deckCenterX - (GameLayoutConfig::kCardWidth * 0.5f);
}
}

void GameController::init(GameView* gameView)
{
    _gameView = gameView;
    bindViewCallbacks();
}

bool GameController::startGame(const std::string& levelPath)
{
    if (_gameView == nullptr)
    {
        return false;
    }

    LevelConfig levelConfig;
    if (!LevelConfigLoader::loadFromFile(levelPath, levelConfig))
    {
        return false;
    }

    _gameModel = GameModelFromLevelGenerator::generate(levelConfig);
    _currentLevelPath = levelPath;
    _currentLevelIndex = findLevelIndex(levelPath);
    _undoManager.bindModel(&_gameModel.undoModel);
    layoutStackCards();
    _gameView->buildFromModel(_gameModel);
    refreshViewFromModel();
    return true;
}

void GameController::onUndoClicked()
{
    UndoRecord record;
    if (!_undoManager.popRecord(record))
    {
        return;
    }

    CardModel* movedCard = _gameModel.getCard(record.movedCardId);
    if (movedCard == nullptr)
    {
        return;
    }

    movedCard->position = record.movedFrom;
    movedCard->isInPlayField = record.movedFromPlayField;

    if (record.movedFromPlayField)
    {
        if (std::find(_gameModel.playFieldCardIds.begin(), _gameModel.playFieldCardIds.end(), movedCard->cardId) == _gameModel.playFieldCardIds.end())
        {
            _gameModel.playFieldCardIds.push_back(movedCard->cardId);
        }
    }
    else
    {
        auto iter = std::find(_gameModel.playFieldCardIds.begin(), _gameModel.playFieldCardIds.end(), movedCard->cardId);
        if (iter != _gameModel.playFieldCardIds.end())
        {
            _gameModel.playFieldCardIds.erase(iter);
        }
        _gameModel.stackCardIds.insert(_gameModel.stackCardIds.begin(), movedCard->cardId);
    }

    _gameModel.topCardId = record.previousTopCardId;
    layoutStackCards();

    refreshViewFromModel();
    _gameView->animateCardMove(movedCard->cardId, record.movedTo, record.movedFrom, [this]()
    {
        _gameView->refreshTopCard(_gameModel.getCard(_gameModel.topCardId));
    });
}

void GameController::onPlayFieldCardClicked(int cardId)
{
    if (!CardController::canSelectPlayFieldCard(_gameModel, cardId))
    {
        return;
    }

    CardModel* clickedCard = _gameModel.getCard(cardId);
    CardModel* topCard = _gameModel.getCard(_gameModel.topCardId);
    if (clickedCard == nullptr || topCard == nullptr)
    {
        return;
    }

    if (!CardRuleService::canMatch(topCard->face, clickedCard->face))
    {
        return;
    }

    const Vec2 moveFrom = clickedCard->position;
    const Vec2 moveTo = topCard->position;

    UndoRecord record;
    record.actionType = UndoActionType::ReplaceTopCard;
    record.movedCardId = clickedCard->cardId;
    record.previousTopCardId = _gameModel.topCardId;
    record.movedFrom = moveFrom;
    record.movedTo = moveTo;
    record.movedFromPlayField = true;
    _undoManager.pushRecord(record);

    clickedCard->position = moveTo;
    clickedCard->isInPlayField = false;
    _gameModel.topCardId = clickedCard->cardId;

    auto iter = std::find(_gameModel.playFieldCardIds.begin(), _gameModel.playFieldCardIds.end(), clickedCard->cardId);
    if (iter != _gameModel.playFieldCardIds.end())
    {
        _gameModel.playFieldCardIds.erase(iter);
    }

    refreshViewFromModel();
    _gameView->animateCardMove(clickedCard->cardId, moveFrom, moveTo, [this]()
    {
        _gameView->refreshTopCard(_gameModel.getCard(_gameModel.topCardId));
    });
}

void GameController::onStackClicked()
{
    if (_gameModel.stackCardIds.empty())
    {
        return;
    }

    CardModel* previousTopCard = _gameModel.getCard(_gameModel.topCardId);
    const int newTopId = _gameModel.stackCardIds.front();
    _gameModel.stackCardIds.erase(_gameModel.stackCardIds.begin());

    CardModel* newTopCard = _gameModel.getCard(newTopId);
    if (newTopCard == nullptr)
    {
        return;
    }

    UndoRecord record;
    record.actionType = UndoActionType::ReplaceTopCard;
    record.movedCardId = newTopCard->cardId;
    record.previousTopCardId = _gameModel.topCardId;
    record.movedFrom = newTopCard->position;
    record.movedTo = previousTopCard != nullptr ? previousTopCard->position : GameLayoutConfig::getTopCardAnchorPosition();
    record.movedFromPlayField = false;
    _undoManager.pushRecord(record);

    const Vec2 moveFrom = record.movedFrom;
    const Vec2 moveTo = record.movedTo;

    newTopCard->position = record.movedTo;
    newTopCard->isInPlayField = false;
    _gameModel.topCardId = newTopCard->cardId;
    layoutStackCards();

    refreshViewFromModel();
    _gameView->animateCardMove(newTopCard->cardId, moveFrom, moveTo, [this]()
    {
        _gameView->refreshTopCard(_gameModel.getCard(_gameModel.topCardId));
    });
}

void GameController::bindViewCallbacks()
{
    if (_gameView == nullptr)
    {
        return;
    }

    _gameView->setOnPlayFieldCardClicked([this](int cardId)
    {
        this->onPlayFieldCardClicked(cardId);
    });

    _gameView->setOnStackClicked([this]()
    {
        this->onStackClicked();
    });

    _gameView->setOnUndoClicked([this]()
    {
        this->onUndoClicked();
    });

    _gameView->setOnRestartClicked([this]()
    {
        this->onRestartRequested();
    });

    _gameView->setOnNextLevelClicked([this]()
    {
        this->onNextLevelRequested();
    });

    _gameView->setOnCloseGameClicked([this]()
    {
        this->onCloseGameRequested();
    });
}

void GameController::onRestartRequested()
{
    if (_currentLevelPath.empty())
    {
        return;
    }

    startGame(_currentLevelPath);
}

void GameController::onCloseGameRequested()
{
    Director::getInstance()->end();
}

void GameController::onNextLevelRequested()
{
    if (_currentLevelIndex < 0)
    {
        return;
    }

    const int nextLevelIndex = _currentLevelIndex + 1;
    if (nextLevelIndex < 0 || nextLevelIndex >= static_cast<int>(kLevelPaths.size()))
    {
        return;
    }

    startGame(kLevelPaths[nextLevelIndex]);
}

Vec2 GameController::getTopCardPosition() const
{
    const CardModel* topCard = _gameModel.getCard(_gameModel.topCardId);
    if (topCard == nullptr)
    {
        return GameLayoutConfig::getTopCardAnchorPosition();
    }

    return topCard->position;
}

int GameController::getStackTopCardId() const
{
    if (_gameModel.stackCardIds.empty())
    {
        return -1;
    }

    return _gameModel.stackCardIds.front();
}

void GameController::layoutStackCards()
{
    const float stackStartX = getStackStartX();
    const int stackCount = static_cast<int>(_gameModel.stackCardIds.size());
    for (size_t i = 0; i < _gameModel.stackCardIds.size(); ++i)
    {
        CardModel* stackCard = _gameModel.getCard(_gameModel.stackCardIds[i]);
        if (stackCard == nullptr)
        {
            continue;
        }

        stackCard->position = Vec2(stackStartX - kStackOffsetX * static_cast<float>(i), kStackY);
        stackCard->isInPlayField = false;
        stackCard->renderOrder = stackCount - static_cast<int>(i);
    }
}

bool GameController::shouldShowCard(int cardId) const
{
    const CardModel* card = _gameModel.getCard(cardId);
    if (card == nullptr)
    {
        return false;
    }

    const bool isInStack = std::find(_gameModel.stackCardIds.begin(), _gameModel.stackCardIds.end(), cardId) != _gameModel.stackCardIds.end();
    return card->isInPlayField || isInStack || cardId == _gameModel.topCardId;
}

bool GameController::isGameWon() const
{
    return _gameModel.playFieldCardIds.empty();
}

bool GameController::hasAnyPlayableMatch() const
{
    const CardModel* topCard = _gameModel.getCard(_gameModel.topCardId);
    if (topCard == nullptr)
    {
        return false;
    }

    for (int cardId : _gameModel.playFieldCardIds)
    {
        if (!CardController::canSelectPlayFieldCard(_gameModel, cardId))
        {
            continue;
        }

        const CardModel* card = _gameModel.getCard(cardId);
        if (card != nullptr && CardRuleService::canMatch(topCard->face, card->face))
        {
            return true;
        }
    }

    return false;
}

bool GameController::isGameOver() const
{
    return !_gameModel.playFieldCardIds.empty() && _gameModel.stackCardIds.empty() && !hasAnyPlayableMatch();
}

bool GameController::hasNextLevel() const
{
    if (_currentLevelIndex < 0)
    {
        return false;
    }

    return _currentLevelIndex + 1 < static_cast<int>(kLevelPaths.size());
}

void GameController::refreshViewFromModel()
{
    if (_gameView == nullptr)
    {
        return;
    }

    _gameView->setStackTopCardId(getStackTopCardId());
    _gameView->refreshStackInfo(static_cast<int>(_gameModel.stackCardIds.size()));
    for (const auto& pair : _gameModel.cards)
    {
        _gameView->syncCard(pair.second);
        _gameView->setCardVisible(pair.first, shouldShowCard(pair.first));
    }

    _gameView->refreshTopCard(_gameModel.getCard(_gameModel.topCardId));
    if (isGameWon())
    {
        _gameView->showGameWinDialog(hasNextLevel());
    }
    else if (isGameOver())
    {
        _gameView->showGameOverDialog();
    }
    else
    {
        _gameView->hideResultDialogs();
    }
}
