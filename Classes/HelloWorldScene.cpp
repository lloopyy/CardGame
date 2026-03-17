#include "HelloWorldScene.h"
#include "views/GameView.h"
#include <algorithm>

namespace
{
constexpr float kMenuButtonWidth = 520.0f;
constexpr float kMenuButtonHeight = 120.0f;

bool attachCloseMenu(HelloWorld* scene)
{
    const auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    const cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();

    auto closeItem = cocos2d::MenuItemImage::create(
        "CloseNormal.png",
        "CloseSelected.png",
        CC_CALLBACK_1(HelloWorld::menuCloseCallback, scene));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        printf("Error while loading: 'CloseNormal.png' and 'CloseSelected.png'\n");
        return false;
    }

    const float x = origin.x + visibleSize.width - closeItem->getContentSize().width / 2;
    const float y = origin.y + closeItem->getContentSize().height / 2;
    closeItem->setPosition(cocos2d::Vec2(x, y));

    auto menu = cocos2d::Menu::create(closeItem, nullptr);
    menu->setPosition(cocos2d::Vec2::ZERO);
    scene->addChild(menu, 100);
    return true;
}

void addFramedButton(cocos2d::Node* parent,
                     const cocos2d::Vec2& center,
                     const cocos2d::Size& size)
{
    if (parent == nullptr)
    {
        return;
    }

    auto fill = cocos2d::LayerColor::create(cocos2d::Color4B(255, 255, 255, 160), size.width, size.height);
    fill->setIgnoreAnchorPointForPosition(false);
    fill->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
    fill->setPosition(center);
    parent->addChild(fill, 1);

    const float halfW = size.width * 0.5f;
    const float halfH = size.height * 0.5f;
    cocos2d::Vec2 vertices[4] = {
        cocos2d::Vec2(center.x - halfW, center.y - halfH),
        cocos2d::Vec2(center.x + halfW, center.y - halfH),
        cocos2d::Vec2(center.x + halfW, center.y + halfH),
        cocos2d::Vec2(center.x - halfW, center.y + halfH)
    };

    auto border = cocos2d::DrawNode::create();
    border->drawPoly(vertices, 4, true, cocos2d::Color4F::WHITE);
    border->setLineWidth(2.0f);
    parent->addChild(border, 2);
}

void attachScaledStartBackground(cocos2d::Node* layer,
                                 const cocos2d::Size& visibleSize,
                                 const cocos2d::Vec2& center)
{
    auto bg = cocos2d::Sprite::create("res/start.png");
    if (bg == nullptr)
    {
        return;
    }

    const cocos2d::Size imageSize = bg->getContentSize();
    if (imageSize.width > 0.0f && imageSize.height > 0.0f)
    {
        const float scale = std::max(visibleSize.width / imageSize.width,
                                     visibleSize.height / imageSize.height);
        bg->setScale(scale);
    }

    bg->setPosition(center);
    layer->addChild(bg, 0);
}

cocos2d::MenuItemLabel* createFramedMenuItem(cocos2d::Node* layer,
                                             const std::string& text,
                                             const cocos2d::Vec2& pos,
                                             const cocos2d::ccMenuCallback& callback)
{
    addFramedButton(layer, pos, cocos2d::Size(kMenuButtonWidth, kMenuButtonHeight));

    auto label = cocos2d::Label::createWithSystemFont(text, "Arial", 56);
    label->setTextColor(cocos2d::Color4B::BLACK);
    auto item = cocos2d::MenuItemLabel::create(label, callback);
    item->setPosition(pos);
    return item;
}
}

cocos2d::Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    if (!cocos2d::Scene::init())
    {
        return false;
    }

    if (!attachCloseMenu(this))
    {
        return false;
    }

    createStartLayer();

    return true;
}

void HelloWorld::createStartLayer()
{
    const auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    const cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();
    const cocos2d::Vec2 center = origin + visibleSize * 0.5f;

    auto layer = cocos2d::Node::create();

    auto bg = cocos2d::Sprite::create("res/start.png");
    if (bg != nullptr)
    {
        const cocos2d::Size imageSize = bg->getContentSize();
        if (imageSize.width > 0.0f && imageSize.height > 0.0f)
        {
            const float scale = std::max(visibleSize.width / imageSize.width,
                                         visibleSize.height / imageSize.height);
            bg->setScale(scale);
        }
        bg->setPosition(center);
        layer->addChild(bg, 0);
    }

    const cocos2d::Vec2 startPosition(center.x, center.y + 80.0f);
    const cocos2d::Vec2 closePosition(center.x, center.y - 40.0f);

    addFramedButton(layer, startPosition, cocos2d::Size(520.0f, 120.0f));
    addFramedButton(layer, closePosition, cocos2d::Size(520.0f, 120.0f));

    auto startLabel = cocos2d::Label::createWithSystemFont("Start Game", "Arial", 56);
    startLabel->setTextColor(cocos2d::Color4B::BLACK);
    auto startItem = cocos2d::MenuItemLabel::create(startLabel, CC_CALLBACK_1(HelloWorld::menuStartGameCallback, this));
    startItem->setPosition(startPosition);

    auto closeLabel = cocos2d::Label::createWithSystemFont("Close Game", "Arial", 56);
    closeLabel->setTextColor(cocos2d::Color4B::BLACK);
    auto closeItem = cocos2d::MenuItemLabel::create(closeLabel, CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    closeItem->setPosition(closePosition);

    auto menu = cocos2d::Menu::create(startItem, closeItem, nullptr);
    menu->setPosition(cocos2d::Vec2::ZERO);
    layer->addChild(menu, 3);

    addChild(layer, 200);
    _startLayer = layer;
}

void HelloWorld::createLevelSelectLayer()
{
    const auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    const cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();
    const cocos2d::Vec2 center = origin + visibleSize * 0.5f;

    auto layer = cocos2d::Node::create();
    attachScaledStartBackground(layer, visibleSize, center);

    auto title = cocos2d::Label::createWithSystemFont("Select Level", "Arial", 58);
    title->setTextColor(cocos2d::Color4B::WHITE);
    title->setPosition(cocos2d::Vec2(center.x, center.y + 280.0f));
    layer->addChild(title, 3);

    const cocos2d::Vec2 level1Pos(center.x, center.y + 100.0f);
    const cocos2d::Vec2 level2Pos(center.x, center.y - 20.0f);
    const cocos2d::Vec2 backPos(center.x, center.y - 140.0f);

    auto level1Item = createFramedMenuItem(layer,
                                           "Level 1",
                                           level1Pos,
                                           CC_CALLBACK_1(HelloWorld::menuSelectLevel1Callback, this));
    auto level2Item = createFramedMenuItem(layer,
                                           "Level 2",
                                           level2Pos,
                                           CC_CALLBACK_1(HelloWorld::menuSelectLevel2Callback, this));
    auto backItem = createFramedMenuItem(layer,
                                         "Back",
                                         backPos,
                                         CC_CALLBACK_1(HelloWorld::menuBackToStartCallback, this));

    auto menu = cocos2d::Menu::create(level1Item, level2Item, backItem, nullptr);
    menu->setPosition(cocos2d::Vec2::ZERO);
    layer->addChild(menu, 3);

    addChild(layer, 200);
    _levelSelectLayer = layer;
}

void HelloWorld::clearStartMenus()
{
    if (_startLayer != nullptr)
    {
        _startLayer->removeFromParent();
        _startLayer = nullptr;
    }

    if (_levelSelectLayer != nullptr)
    {
        _levelSelectLayer->removeFromParent();
        _levelSelectLayer = nullptr;
    }
}

bool HelloWorld::attachGameLayer(const std::string& levelPath)
{
    auto gameView = GameView::create();
    if (gameView == nullptr)
    {
        return false;
    }

    addChild(gameView, 1);
    _gameController.init(gameView);

    const bool loaded = _gameController.startGame(levelPath);
    if (!loaded)
    {
        CCLOG("Failed to load level file: %s", levelPath.c_str());
    }
    return loaded;
}

void HelloWorld::menuStartGameCallback(cocos2d::Ref* pSender)
{
    clearStartMenus();
    createLevelSelectLayer();
}

void HelloWorld::menuSelectLevel1Callback(cocos2d::Ref* pSender)
{
    if (!attachGameLayer("res/levels/level_1.json"))
    {
        return;
    }

    clearStartMenus();
}

void HelloWorld::menuSelectLevel2Callback(cocos2d::Ref* pSender)
{
    if (!attachGameLayer("res/levels/level_2.json"))
    {
        return;
    }

    clearStartMenus();
}

void HelloWorld::menuBackToStartCallback(cocos2d::Ref* pSender)
{
    clearStartMenus();
    createStartLayer();
}


void HelloWorld::menuCloseCallback(cocos2d::Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    cocos2d::Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}
