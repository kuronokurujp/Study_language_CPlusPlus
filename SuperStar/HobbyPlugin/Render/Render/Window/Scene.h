#pragma once

#include "Engine/Common/PoolManager.h"
#include "Engine/MiniEngine.h"
#include "Render/Command/CommandBuffer.h"

// 前方宣言
namespace Platform
{
    class SceneStrategyInterface;
}

namespace Render::Prticle
{
    class Blob;
}

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

        using RuntimePoolPricleBlob = Core::Common::RuntimePoolManager<Render::Prticle::Blob>;

    public:
        SceneViewBase();
        virtual ~SceneViewBase() = default;
        void Release();

        const Render::Prticle::Blob& GetPrticle(const Core::Common::Handle);

    protected:
        HE::Bool _Begin(Core::Memory::UniquePtr<Platform::SceneStrategyInterface>);
        void _End();
        void _Update(const HE::Float32);

        void _BeginRender();
        void _EndRender();

        // 描画コマンドを追加
        // コマンドデータは呼び出し元のものにするためにstd::move()で渡す
        HE::Bool _PushCommand(Command&& in_rrCmd);

    protected:
        // TODO: 描画に必要なリソースをアクセスするようにしている
        Core::Memory::UniquePtr<class Platform::SceneStrategyInterface> _upSt;

        CommandBuffer _commandBuff;

    private:
        RuntimePoolPricleBlob _runtimePoolPrticleBlob;
    };

}  // namespace Render
