#ifndef __CARD_VIEW_H__
#define __CARD_VIEW_H__

#include "cocos2d.h"
#include "configs/models/GameLayoutConfig.h"
#include "models/CardModel.h"
#include <functional>

/**
 * @brief 卡牌视图组件，负责单张卡牌展示和点击输入。
 */
class CardView : public cocos2d::Node
{
public:
    static CardView* create(const CardModel& model);

    bool initWithModel(const CardModel& model);
    void setModel(const CardModel& model);
    const CardModel& getModel() const;

    void setOnClick(const std::function<void(int)>& callback);

private:
    void refreshVisual();
    void layoutCardSprites();
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);

private:
    CardModel _model;
    cocos2d::Sprite* _cardBackground = nullptr;
    cocos2d::Sprite* _smallNumber = nullptr;
    cocos2d::Sprite* _suitIcon = nullptr;
    cocos2d::Sprite* _bigNumber = nullptr;
    std::function<void(int)> _onClick;
};

#endif // __CARD_VIEW_H__
