#ifndef __GAME_CONTROLLER_H__
#define __GAME_CONTROLLER_H__

#include "managers/UndoManager.h"
#include "models/GameModel.h"
#include <string>

class GameView;

/**
 * @brief 游戏主控制器，负责协调模型、视图和业务流程。
 */
class GameController
{
public:
    /**
     * @brief 初始化控制器并绑定视图。
     */
    void init(GameView* gameView);

    /**
     * @brief 启动游戏并加载指定关卡配置。
     */
    bool startGame(const std::string& levelPath);

    /**
     * @brief 处理回退按钮点击。
     */
    void onUndoClicked();

private:
    void onPlayFieldCardClicked(int cardId);
    void onStackClicked();
    void onRestartRequested();
    void onNextLevelRequested();
    void onCloseGameRequested();
    void bindViewCallbacks();
    cocos2d::Vec2 getTopCardPosition() const;
    int getStackTopCardId() const;
    void layoutStackCards();
    bool shouldShowCard(int cardId) const;
    bool hasAnyPlayableMatch() const;
    bool isGameWon() const;
    bool isGameOver() const;
    bool hasNextLevel() const;
    void refreshViewFromModel();

private:
    GameModel _gameModel;
    UndoManager _undoManager;
    GameView* _gameView = nullptr;
    std::string _currentLevelPath;
    int _currentLevelIndex = -1;
};

#endif // __GAME_CONTROLLER_H__
