#include "views/GameView.h"

#include "configs/models/GameLayoutConfig.h"
#include "utils/CardUtils.h"
#include <algorithm>
#include <string>

USING_NS_CC;

namespace
{
constexpr float kDialogPanelWidth = 700.0f;
constexpr float kDialogPanelHeight = 620.0f;

std::string toSuitWord(CardSuitType suit)
{
    switch (suit)
    {
    case CST_SPADES: return "Spade";
    case CST_CLUBS: return "Club";
    case CST_HEARTS: return "Heart";
    case CST_DIAMONDS: return "Diamond";
    default: return "";
    }
}

MenuItemLabel* createDialogItem(const std::string& text,
                                const Vec2& position,
                                const std::function<void()>& onClick)
{
    auto label = Label::createWithSystemFont(text, "Arial", 36);
    label->setTextColor(Color4B::BLACK);

    auto item = MenuItemLabel::create(label, [onClick](Ref*)
    {
        if (onClick)
        {
            onClick();
        }
    });
    item->setPosition(position);
    return item;
}

Node* createDialogOverlay(const Size& visibleSize, const Vec2& origin)
{
    auto overlay = LayerColor::create(Color4B(0, 0, 0, 140), visibleSize.width, visibleSize.height);
    overlay->setAnchorPoint(Vec2::ZERO);
    overlay->setPosition(origin);
    return overlay;
}

Node* createDialogPanel(const Size& visibleSize, const Vec2& origin)
{
    auto panel = LayerColor::create(Color4B(248, 248, 240, 255), kDialogPanelWidth, kDialogPanelHeight);
    panel->setIgnoreAnchorPointForPosition(false);
    panel->setAnchorPoint(Vec2(0.5f, 0.5f));
    panel->setPosition(origin + visibleSize * 0.5f);
    return panel;
}

void addDialogTexts(Node* panel, const std::string& titleText, const std::string& hintText)
{
    auto title = Label::createWithSystemFont(titleText, "Arial", 58);
    title->setTextColor(Color4B(28, 84, 24, 255));
    title->setPosition(Vec2(kDialogPanelWidth * 0.5f, 500.0f));
    panel->addChild(title);

    auto hint = Label::createWithSystemFont(hintText, "Arial", 30);
    hint->setTextColor(Color4B(40, 40, 40, 255));
    hint->setPosition(Vec2(kDialogPanelWidth * 0.5f, 420.0f));
    panel->addChild(hint);
}
}

bool GameView::init()
{
    if (!Layer::init())
    {
        return false;
    }

    createBackgroundAreas();
    createInfoLabels();
    createButtons();
    return true;
}

void GameView::createBackgroundAreas()
{
    auto bg = LayerColor::create(Color4B(12, 70, 24, 255));
    addChild(bg, -2);

    const Size visibleSize = Director::getInstance()->getVisibleSize();
    const Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto fullBgImage = Sprite::create("res/background.png");
    if (fullBgImage != nullptr)
    {
        const Size imageSize = fullBgImage->getContentSize();
        if (imageSize.width > 0.0f && imageSize.height > 0.0f)
        {
            const float scale = std::max(visibleSize.width / imageSize.width,
                                         visibleSize.height / imageSize.height);
            fullBgImage->setScale(scale);
        }

        fullBgImage->setAnchorPoint(Vec2(0.5f, 0.5f));
        fullBgImage->setPosition(origin + visibleSize * 0.5f);
        addChild(fullBgImage, -1);
    }

    const Rect stackRect = GameLayoutConfig::getStackRect();

    auto stackArea = LayerColor::create(Color4B(18, 88, 30, 140), stackRect.size.width, stackRect.size.height);
    stackArea->setAnchorPoint(Vec2::ZERO);
    stackArea->setPosition(stackRect.origin);
    addChild(stackArea, 0);
}

void GameView::createInfoLabels()
{
    const Vec2 drawAnchor = GameLayoutConfig::getDrawStackAnchorPosition();
    const float deckCenterX = drawAnchor.x + 24.0f + (GameLayoutConfig::kCardWidth * 0.5f);
    const float deckLabelY = drawAnchor.y - 48.0f;

    const Vec2 topAnchor = GameLayoutConfig::getTopCardAnchorPosition();
    const float topCenterX = topAnchor.x + (GameLayoutConfig::kCardWidth * 0.5f);
    const float topLabelY = topAnchor.y - 48.0f;

    _stackCountLabel = Label::createWithSystemFont("x0", "Arial", 36);
    _stackCountLabel->setAnchorPoint(Vec2(0.5f, 0.5f));
    _stackCountLabel->setPosition(Vec2(deckCenterX, deckLabelY));
    addChild(_stackCountLabel, 5);

    _stackEmptyHintLabel = Label::createWithSystemFont("Deck is empty", "Arial", 32);
    _stackEmptyHintLabel->setTextColor(Color4B(255, 235, 170, 255));
    _stackEmptyHintLabel->setPosition(Vec2(deckCenterX, deckLabelY - 42.0f));
    _stackEmptyHintLabel->setVisible(false);
    addChild(_stackEmptyHintLabel, 5);

    _topCardLabel = Label::createWithSystemFont("Top: -", "Arial", 36);
    _topCardLabel->setAnchorPoint(Vec2(0.5f, 0.5f));
    _topCardLabel->setPosition(Vec2(topCenterX, topLabelY));
    addChild(_topCardLabel, 5);
}

void GameView::buildFromModel(const GameModel& model)
{
    clearCardViews();

    _stackTopCardId = model.stackCardIds.empty() ? -1 : model.stackCardIds.front();

    for (const auto& pair : model.cards)
    {
        addCardView(model, pair.first, pair.second);
    }

    const CardModel* top = model.getCard(model.topCardId);
    refreshTopCard(top);
}

void GameView::clearCardViews()
{
    for (auto& pair : _cardViews)
    {
        if (pair.second != nullptr)
        {
            pair.second->removeFromParent();
        }
    }
    _cardViews.clear();
}

void GameView::addCardView(const GameModel& model, int cardId, const CardModel& card)
{
    CardView* view = CardView::create(card);
    if (view == nullptr)
    {
        return;
    }

    view->setOnClick([this](int clickedCardId)
    {
        handleCardViewClick(clickedCardId);
    });

    const bool isTopCard = (cardId == model.topCardId);
    const bool isStackTopCard = (cardId == _stackTopCardId);
    const bool shouldShow = card.isInPlayField || isTopCard || isStackTopCard;
    view->setVisible(shouldShow);

    addChild(view);
    _cardViews[cardId] = view;
}

void GameView::handleCardViewClick(int clickedCardId)
{
    CardView* clickedView = getCardView(clickedCardId);
    if (clickedView == nullptr)
    {
        return;
    }

    const CardModel& cardModel = clickedView->getModel();
    if (cardModel.isInPlayField)
    {
        if (_onPlayFieldCardClicked)
        {
            _onPlayFieldCardClicked(clickedCardId);
        }
        return;
    }

    if (clickedCardId == _stackTopCardId && _onStackClicked)
    {
        _onStackClicked();
    }
}

void GameView::syncCard(const CardModel& card)
{
    CardView* view = getCardView(card.cardId);
    if (view == nullptr)
    {
        return;
    }

    view->setVisible(true);
    view->setModel(card);
}

void GameView::setCardVisible(int cardId, bool visible)
{
    CardView* view = getCardView(cardId);
    if (view == nullptr)
    {
        return;
    }

    view->setVisible(visible);
}

void GameView::setStackTopCardId(int cardId)
{
    _stackTopCardId = cardId;
}

void GameView::refreshStackInfo(int remainingCount)
{
    if (_stackCountLabel != nullptr)
    {
        _stackCountLabel->setString("x" + std::to_string(std::max(0, remainingCount)));
    }

    if (_stackEmptyHintLabel != nullptr)
    {
        _stackEmptyHintLabel->setVisible(remainingCount <= 0);
    }
}

void GameView::refreshTopCard(const CardModel* topCard)
{
    if (topCard == nullptr)
    {
        _topCardLabel->setString("Top: -");
        return;
    }

    const std::string suitWord = toSuitWord(topCard->suit);
    const std::string faceText = CardUtils::faceToString(topCard->face);
    _topCardLabel->setString("Top: " + suitWord + faceText);
}

void GameView::showGameWinDialog(bool showNextLevelButton)
{
    if (_winDialog == nullptr)
    {
        _winDialog = createResultDialog("YOU WIN", "All playfield cards are cleared.", true, &_winNextLevelItem);
        addChild(_winDialog, 1200);
    }

    if (_winNextLevelItem != nullptr)
    {
        _winNextLevelItem->setVisible(showNextLevelButton);
    }

    _winDialog->setVisible(true);
    if (_gameOverDialog != nullptr)
    {
        _gameOverDialog->setVisible(false);
    }
}

void GameView::showGameOverDialog()
{
    if (_gameOverDialog == nullptr)
    {
        _gameOverDialog = createResultDialog("GAME OVER", "No more matches and deck is empty.", false, nullptr);
        addChild(_gameOverDialog, 1000);
    }

    _gameOverDialog->setVisible(true);
    if (_winDialog != nullptr)
    {
        _winDialog->setVisible(false);
    }
}

void GameView::hideResultDialogs()
{
    if (_winDialog != nullptr)
    {
        _winDialog->setVisible(false);
    }

    if (_gameOverDialog != nullptr)
    {
        _gameOverDialog->setVisible(false);
    }
}

void GameView::animateCardMove(int cardId,
                               const Vec2& from,
                               const Vec2& target,
                               const std::function<void()>& onComplete)
{
    CardView* view = getCardView(cardId);
    if (view == nullptr)
    {
        if (onComplete)
        {
            onComplete();
        }
        return;
    }

    view->stopAllActions();
    view->setPosition(from);
    view->runAction(Sequence::create(
        MoveTo::create(0.2f, target),
        CallFunc::create([onComplete]()
        {
            if (onComplete)
            {
                onComplete();
            }
        }),
        nullptr));
}

void GameView::setOnPlayFieldCardClicked(const std::function<void(int)>& callback)
{
    _onPlayFieldCardClicked = callback;
}

void GameView::setOnStackClicked(const std::function<void()>& callback)
{
    _onStackClicked = callback;
}

void GameView::setOnUndoClicked(const std::function<void()>& callback)
{
    _onUndoClicked = callback;
}

void GameView::setOnRestartClicked(const std::function<void()>& callback)
{
    _onRestartClicked = callback;
}

void GameView::setOnCloseGameClicked(const std::function<void()>& callback)
{
    _onCloseGameClicked = callback;
}

void GameView::setOnNextLevelClicked(const std::function<void()>& callback)
{
    _onNextLevelClicked = callback;
}

CardView* GameView::getCardView(int cardId) const
{
    auto iter = _cardViews.find(cardId);
    if (iter == _cardViews.end())
    {
        return nullptr;
    }

    return iter->second;
}

void GameView::createButtons()
{
    auto stackItem = MenuItemLabel::create(Label::createWithSystemFont("Draw", "Arial", 40), [this](Ref*)
    {
        if (_onStackClicked)
        {
            _onStackClicked();
        }
    });
    stackItem->setPosition(Vec2(300.0f, 110.0f));

    auto undoItem = MenuItemLabel::create(Label::createWithSystemFont("Undo", "Arial", 40), [this](Ref*)
    {
        if (_onUndoClicked)
        {
            _onUndoClicked();
        }
    });
    undoItem->setPosition(Vec2(750.0f, 110.0f));

    auto menu = Menu::create(stackItem, undoItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    addChild(menu, 10);
}

Node* GameView::createResultDialog(const std::string& titleText,
                                   const std::string& hintText,
                                   bool withNextLevelButton,
                                   MenuItemLabel** outNextLevelItem)
{
    const Size visibleSize = Director::getInstance()->getVisibleSize();
    const Vec2 origin = Director::getInstance()->getVisibleOrigin();

    Node* overlay = createDialogOverlay(visibleSize, origin);
    Node* panel = createDialogPanel(visibleSize, origin);
    overlay->addChild(panel);

    addDialogTexts(panel, titleText, hintText);

    auto replayItem = createDialogItem("Replay", Vec2(kDialogPanelWidth * 0.5f, 250.0f), [this]()
    {
        if (_onRestartClicked)
        {
            _onRestartClicked();
        }
    });

    MenuItemLabel* nextItem = nullptr;
    if (withNextLevelButton)
    {
        nextItem = createDialogItem("Next Level", Vec2(kDialogPanelWidth * 0.5f, 170.0f), [this]()
        {
            if (_onNextLevelClicked)
            {
                _onNextLevelClicked();
            }
        });
    }

    auto closeItem = createDialogItem("Close Game", Vec2(kDialogPanelWidth * 0.5f, withNextLevelButton ? 90.0f : 150.0f), [this]()
    {
        if (_onCloseGameClicked)
        {
            _onCloseGameClicked();
        }
    });

    auto menu = withNextLevelButton
        ? Menu::create(replayItem, nextItem, closeItem, nullptr)
        : Menu::create(replayItem, closeItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    panel->addChild(menu);

    if (outNextLevelItem != nullptr)
    {
        *outNextLevelItem = nextItem;
    }

    return overlay;
}
