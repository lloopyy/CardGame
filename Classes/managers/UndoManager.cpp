#include "managers/UndoManager.h"

void UndoManager::pushRecord(const UndoRecord& record)
{
    if (_model == nullptr)
    {
        return;
    }

    _model->records.push_back(record);
}

bool UndoManager::popRecord(UndoRecord& outRecord)
{
    if (_model == nullptr || _model->records.empty())
    {
        return false;
    }

    outRecord = _model->records.back();
    _model->records.pop_back();
    return true;
}

bool UndoManager::canUndo() const
{
    return _model != nullptr && !_model->records.empty();
}
