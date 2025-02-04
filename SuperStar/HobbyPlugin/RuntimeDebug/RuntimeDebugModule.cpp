#include "RuntimeDebugModule.h"
namespace RuntimeDebug
{
    /// <summary>
    /// モジュール初期化
    /// </summary>
    HE::Bool RuntimeDebugModule::_VStart()
    {
        return TRUE;
    }
    /// <summary>
    /// インスタンス破棄時に呼ばれる
    /// </summary>
    HE::Bool RuntimeDebugModule::_VRelease()
    {
        return TRUE;
    }
}  // namespace RuntimeDebug
