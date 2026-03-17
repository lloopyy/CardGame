#ifndef __UNDO_MANAGER_H__
#define __UNDO_MANAGER_H__

#include "models/UndoModel.h"

/**
 * @brief 回退管理器，负责回退记录管理。
 */
class UndoManager
{
public:
    /**
     * @brief 初始化时绑定一个撤销模型。
     */
    void bindModel(UndoModel* model)
    {
        _model = model;
    }

    /**
     * @brief 压入一条回退记录。
     */
    void pushRecord(const UndoRecord& record);

    /**
     * @brief 弹出最后一条回退记录。
     */
    bool popRecord(UndoRecord& outRecord);

    /**
     * @brief 是否存在可回退记录。
     */
    bool canUndo() const;

private:
    UndoModel* _model = nullptr;
};

#endif // __UNDO_MANAGER_H__
