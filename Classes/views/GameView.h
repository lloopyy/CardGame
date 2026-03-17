#ifndef __GAME_VIEW_H__
#define __GAME_VIEW_H__

#include "cocos2d.h"
#include "models/GameModel.h"
#include "views/CardView.h"
#include <functional>
#include <unordered_map>

/**
 * @brief 游戏主视图，负责整体布局和交互事件转发。
 */
class GameView : public cocos2d::Layer
{
public:
    CREATE_FUNC(GameView);

    /** @brief 初始化主视图。 */
    virtual bool init() override;

    /** @brief 根据模型重建所有卡牌视图。 */
    void buildFromModel(const GameModel& model);
    /** @brief 同步单张卡牌状态到视图。 */
    void syncCard(const CardModel& card);
    /** @brief 设置指定卡牌是否可见。 */
    void setCardVisible(int cardId, bool visible);
    /** @brief 设置当前备用牌堆顶部卡牌 ID。 */
    void setStackTopCardId(int cardId);
    /** @brief 刷新备用牌堆剩余数量显示。 */
    void refreshStackInfo(int remainingCount);
    /** @brief 刷新手牌区顶部卡牌文案。 */
    void refreshTopCard(const CardModel* topCard);
    /** @brief 显示胜利弹窗。 */
    void showGameWinDialog(bool showNextLevelButton);
    /** @brief 显示失败弹窗。 */
    void showGameOverDialog();
    /** @brief 隐藏所有结果弹窗。 */
    void hideResultDialogs();

    /** @brief 播放卡牌移动动画。 */
    void animateCardMove(int cardId,
                         const cocos2d::Vec2& from,
                         const cocos2d::Vec2& target,
                         const std::function<void()>& onComplete);

    /** @brief 注册主牌区卡牌点击回调。 */
    void setOnPlayFieldCardClicked(const std::function<void(int)>& callback);
    /** @brief 注册抽牌点击回调。 */
    void setOnStackClicked(const std::function<void()>& callback);
    /** @brief 注册回退按钮点击回调。 */
    void setOnUndoClicked(const std::function<void()>& callback);
    /** @brief 注册重新开始按钮点击回调。 */
    void setOnRestartClicked(const std::function<void()>& callback);
    /** @brief 注册关闭游戏按钮点击回调。 */
    void setOnCloseGameClicked(const std::function<void()>& callback);
    /** @brief 注册下一关按钮点击回调。 */
    void setOnNextLevelClicked(const std::function<void()>& callback);

private:
    void clearCardViews();
    void addCardView(const GameModel& model, int cardId, const CardModel& card);
    void handleCardViewClick(int clickedCardId);
    CardView* getCardView(int cardId) const;
    void createBackgroundAreas();
    void createInfoLabels();
    void createButtons();
    cocos2d::Node* createResultDialog(const std::string& title,
                                      const std::string& hint,
                                      bool withNextLevelButton,
                                      cocos2d::MenuItemLabel** outNextLevelItem);

private:
    std::unordered_map<int, CardView*> _cardViews;
    cocos2d::Label* _topCardLabel = nullptr;
    cocos2d::Label* _stackCountLabel = nullptr;
    cocos2d::Label* _stackEmptyHintLabel = nullptr;
    cocos2d::Node* _winDialog = nullptr;
    cocos2d::Node* _gameOverDialog = nullptr;
    cocos2d::MenuItemLabel* _winNextLevelItem = nullptr;
    int _stackTopCardId = -1;

    std::function<void(int)> _onPlayFieldCardClicked;
    std::function<void()> _onStackClicked;
    std::function<void()> _onUndoClicked;
    std::function<void()> _onRestartClicked;
    std::function<void()> _onCloseGameClicked;
    std::function<void()> _onNextLevelClicked;
};

#endif // __GAME_VIEW_H__
