#pragma once

// #include <functional>

#include "Engine/Common/CustomMap.h"
#include "Engine/Common/PoolManager.h"
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
        virtual void _VEnd()   = 0;

        virtual void _VUpdate(const HE::Float32) = 0;

        virtual void _VRender(class ViewPort*) = 0;

        // 描画コマンドを追加
        // コマンドデータは呼び出し元のものにするためにstd::move()で渡す
        HE::Bool _PushCommand(Command&& in_rrCmd);

    protected:
        CommandBuffer _commandBuff;
    };

    struct ViewPortConfig
    {
        HE::Uint32 uSceneCount = 0;
        HE::Uint32 uWidth      = 0;
        HE::Uint32 uHeight     = 0;
    };

    class ViewPort : Core::Common::RuntimePoolManager<SceneViewBase>
    {
        friend class Window;

    public:
        template <typename TScene>
        const Core::Common::Handle& AddSceneView2D()
        {
            auto [handle, pScene] = this->_Alloc<TScene>();
            return handle;
        }

        template <typename TScene>
        const Core::Common::Handle& AddSceneViewUI()
        {
            auto [handle, pScene] = this->_Alloc<TScene>();
            return handle;
        }

        void RemoveScene(Core::Common::Handle&);

        SceneViewBase* GetScene(const Core::Common::Handle&);

        const ViewPortConfig* GetConfig();

    private:
        HE::Bool _Setup(Core::Memory::UniquePtr<ViewPortConfig>);
        void _Begin();
        void _End();

    private:
        Core::Memory::UniquePtr<ViewPortConfig> _upConfig;
    };

    class WindowStrategy
    {
        friend class Window;

    public:
        inline HE::Uint32 ViewPortCount() const { return this->_uViewPortCount; }

    protected:
        virtual void _VBegin() = 0;
        virtual void _VEnd()   = 0;

        virtual void _VShow() = 0;

        virtual void _VBeginRender() = 0;
        virtual void _VEndRender()   = 0;

    protected:
        HE::Uint32 _uViewPortCount = 0;
        HE::Uint32 _uWidht         = 0;
        HE::Uint32 _uHeight        = 0;
    };

    /// <summary>
    /// 描画するウィンドウ
    /// </summary>
    class Window final : Core::Common::RuntimePoolManager<ViewPort>
    {
        friend class RenderModule;

    public:
        Core::Common::Handle AddViewPort(Core::Memory::UniquePtr<ViewPortConfig>);
        HE::Bool RemoveViewPort(const Core::Common::Handle&);

        /*
                // TODO: UI用シーン追加
                template <typename TScene>
                Core::Common::Handle AddSceneViewUI(const Uint64 in_ulHash, const Uint64
           in_ulViewPortHash)
                {
                    auto itr       = this->_mViewPortHandle.FindKey(in_ulViewPortHash);
                    auto pViewPort = this->_Ref(itr->data);

                    return pViewPort->AddSceneViewUI<TScene>(in_ulHash);
                }

                // TODO: 2D用シーン追加
                template <typename TScene>
                Core::Common::Handle AddSceneView2D(const Uint64 in_ulHash, const Uint64
           in_ulViewPortHash)
                {
                    auto itr       = this->_mViewPortHandle.FindKey(in_ulViewPortHash);
                    auto pViewPort = this->_Ref(itr->data);

                    return pViewPort->AddSceneView2D<TScene>(in_ulHash);
                }
                */

        // TODO: ウィンドウ表示
        void Show();

    private:
        HE::Bool _Setup(Core::Memory::UniquePtr<WindowStrategy>);
        void _Begin();
        void _End();

        void _Update(const HE::Float32);

        void _Render();

    private:
        Core::Memory::UniquePtr<WindowStrategy> _upStrategy;
        HE::Bool _bShow  = FALSE;
        HE::Bool _bReady = FALSE;
    };

}  // namespace Render
