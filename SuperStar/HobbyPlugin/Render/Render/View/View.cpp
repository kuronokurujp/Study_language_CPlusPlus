#include "View.h"

namespace Render
{
    Bool View::Release()
    {
        this->ClearCmd();
        return TRUE;
    }

    Bool View::PushCmd(const Command&& in_rrCmd)
    {
        // コマンドをコピー
        // 要素を先頭に追加
        // 最後に追加したコマンドは配列先頭に
        // 最初に追加したコマンドは配列末尾に
        this->_commandBuff.PushFront(in_rrCmd);

        return TRUE;
    }

    // 描画コマンドを排出
    const Command* View::PopCmd()
    {
        // 古いコマンドを優先に排出
        return this->_commandBuff.PopBack();
    }

}  // namespace Render
