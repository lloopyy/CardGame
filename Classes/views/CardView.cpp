#include "views/CardView.h"

#include "configs/models/CardResConfig.h"
#include "utils/CardUtils.h"

USING_NS_CC;

namespace
{
float calcFitScale(const Size& rawSize, float maxWidth, float maxHeight)
{
    if (rawSize.width <= 0.0f || rawSize.height <= 0.0f)
    {
        return 1.0f;
    }

    const float scaleX = maxWidth / rawSize.width;
    const float scaleY = maxHeight / rawSize.height;
    return std::min(scaleX, scaleY);
}

void applyTexture(Sprite* sprite, const std::string& path)
{
    if (sprite == nullptr)
    {
        return;
    }

    if (path.empty())
    {
        sprite->setVisible(false);
        return;
    }

    sprite->setTexture(path);
    auto* texture = sprite->getTexture();
    if (texture == nullptr)
    {
        sprite->setVisible(false);
        return;
    }

    const Size texSize = texture->getContentSize();
    sprite->setTextureRect(Rect(0.0f, 0.0f, texSize.width, texSize.height));
    sprite->setVisible(true);
}

void applyVisibleSpriteScale(Sprite* sprite, float maxWidth, float maxHeight, float multiplier = 1.0f)
{
    if (sprite == nullptr || !sprite->isVisible())
    {
        return;
    }

    const float scale = calcFitScale(sprite->getContentSize(), maxWidth, maxHeight) * multiplier;
    sprite->setScale(scale);
}
}

CardView* CardView::create(const CardModel& model)
{
    CardView* ret = new (std::nothrow) CardView();
    if (ret != nullptr && ret->initWithModel(model))
    {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool CardView::initWithModel(const CardModel& model)
{
    if (!Node::init())
    {
        return false;
    }

    setContentSize(Size(GameLayoutConfig::kCardWidth, GameLayoutConfig::kCardHeight));

    _cardBackground = Sprite::create(CardResConfig::getCardFrontPath());
    if (_cardBackground == nullptr)
    {
        return false;
    }

    _cardBackground->setAnchorPoint(Vec2::ZERO);
    _cardBackground->setPosition(Vec2::ZERO);
    addChild(_cardBackground);

    _smallNumber = Sprite::create();
    _smallNumber->setAnchorPoint(Vec2(0.0f, 1.0f));
    addChild(_smallNumber, 1);

    _suitIcon = Sprite::create();
    _suitIcon->setAnchorPoint(Vec2(1.0f, 1.0f));
    addChild(_suitIcon, 1);

    _bigNumber = Sprite::create();
    _bigNumber->setAnchorPoint(Vec2(0.5f, 0.5f));
    addChild(_bigNumber, 1);

    layoutCardSprites();

    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(CardView::onTouchBegan, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    setModel(model);
    return true;
}

void CardView::setModel(const CardModel& model)
{
    _model = model;
    setPosition(model.position);
    setLocalZOrder(_model.isInPlayField ? (100 + _model.renderOrder) : (300 + _model.renderOrder));
    refreshVisual();
}

const CardModel& CardView::getModel() const
{
    return _model;
}

void CardView::setOnClick(const std::function<void(int)>& callback)
{
    _onClick = callback;
}

void CardView::refreshVisual()
{
    applyTexture(_smallNumber, CardResConfig::getSmallNumberPath(_model.suit, _model.face));
    applyTexture(_suitIcon, CardResConfig::getSuitIconPath(_model.suit));
    applyTexture(_bigNumber, CardResConfig::getBigNumberPath(_model.suit, _model.face));

    if (_cardBackground != nullptr)
    {
        _cardBackground->setColor(Color3B::WHITE);
    }

    applyVisibleSpriteScale(_smallNumber, 100.0f, 40.0f);
    applyVisibleSpriteScale(_suitIcon, 42.0f, 42.0f);
    applyVisibleSpriteScale(_bigNumber, 72.0f, 82.0f, 1.2f);

    setName(CardUtils::toCardName(_model.suit, _model.face));
}

void CardView::layoutCardSprites()
{
    if (_cardBackground != nullptr)
    {
        const float bgScaleX = GameLayoutConfig::kCardWidth / _cardBackground->getContentSize().width;
        const float bgScaleY = GameLayoutConfig::kCardHeight / _cardBackground->getContentSize().height;
        _cardBackground->setScaleX(bgScaleX);
        _cardBackground->setScaleY(bgScaleY);
    }

    _smallNumber->setPosition(Vec2(15.0f, GameLayoutConfig::kCardHeight - 16.0f));
    _suitIcon->setPosition(Vec2(GameLayoutConfig::kCardWidth - 9.0f, GameLayoutConfig::kCardHeight - 16.0f));
    _bigNumber->setPosition(Vec2(GameLayoutConfig::kCardWidth * 0.5f, GameLayoutConfig::kCardHeight * 0.41f));
}

bool CardView::onTouchBegan(Touch* touch, Event* event)
{
    if (!isVisible())
    {
        return false;
    }

    const Vec2 local = convertToNodeSpace(touch->getLocation());
    const Rect rect(0.0f, 0.0f, getContentSize().width, getContentSize().height);
    if (!rect.containsPoint(local))
    {
        return false;
    }

    if (_onClick)
    {
        _onClick(_model.cardId);
    }
    return true;
}
