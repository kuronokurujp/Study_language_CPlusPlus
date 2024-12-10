#include "Scene.h"

namespace Render
{
    HE::Bool SceneViewBase::_PushCommand(Command&& in_rrCmd)
    {
        // コマンドをコピー
        // 要素を先頭に追加
        // 最後に追加したコマンドは配列先頭に
        // 最初に追加したコマンドは配列末尾に
        this->_commandBuff.PushFront(in_rrCmd);

        return TRUE;
    }

}  // namespace Render
