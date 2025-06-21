#include "Engine/Task/Task.h"

namespace Core
{
    Task::Task()
    {
        this->_Clear();
    }

    void Task::VSetup(const HE::Bool in_bReleaseMem)
    {
        // パラメータはタスク管理に登録された時に設定される
        this->_bReleaseMem = in_bReleaseMem;
    }

    void Task::Kill()
    {
        // 死亡フラグを付ける
        _bKill = TRUE;
    }
}  // namespace Core
