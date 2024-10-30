#pragma once

#include "Engine/Core.h"
#include "Render/Command/CommandBuffer.h"

namespace Render
{
    /// <summary>
    /// レンダリングするビュー
    /// 描画コマンドをためてコマンドを実行する
    /// </summary>
    class View final
    {
    public:
        Bool Release();

        // 描画コマンドを追加
        // コマンドデータは呼び出し元のものにするためにstd::move()で渡す
        Bool PushCmd(const Command&& in_rrCmd);

        // 描画コマンドを排出
        const Command* PopCmd();

        /// <summary>
        /// コマンドをクリア
        /// </summary>
        inline void ClearCmd() { this->_commandBuff.Clear(); }

        /// <summary>
        /// コマンドバッファを取得
        /// </summary>
        inline const CommandBuffer* GetCmdBuff() const { return &this->_commandBuff; }

    private:
        CommandBuffer _commandBuff;
    };
}  // namespace Render
