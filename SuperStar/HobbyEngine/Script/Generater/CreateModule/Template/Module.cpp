#include "__MODULE_NAME__Module.h"

namespace __MODULE_NAME__
{
    /// <summary>
    /// モジュール初期化
    /// </summary>
    HE::Bool __MODULE_NAME__Module::_VStart()
    {
        return TRUE;
    }

    /// <summary>
    /// インスタンス破棄時に呼ばれる
    /// </summary>
    HE::Bool __MODULE_NAME__Module::_VRelease()
    {
        return TRUE;
    }
}  // namespace __MODULE_NAME__
