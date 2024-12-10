#pragma once

#include "Engine/MiniEngine.h"
#include "Render/Command/CommandBuffer.h"

namespace Render
{
    /// <summary>
    /// 描画するシーンのベース
    /// 継承して2Dや3D用のシーンを作成する
    /// </summary>
    class SceneViewBase
    {
        friend class ViewPort;
        friend class Window;
        friend class RenderModule;

    protected:
        virtual HE::Bool _VBegin() = 0;
        virtual void _VEnd()       = 0;

        virtual void _VUpdate(const HE::Float32) = 0;

        virtual void _VRender(class ViewPort*) = 0;

        // 描画コマンドを追加
        // コマンドデータは呼び出し元のものにするためにstd::move()で渡す
        HE::Bool _PushCommand(Command&& in_rrCmd);

    protected:
        CommandBuffer _commandBuff;
    };

}  // namespace Render
