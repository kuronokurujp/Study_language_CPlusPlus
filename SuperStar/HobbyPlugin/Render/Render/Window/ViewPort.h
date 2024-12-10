#pragma once

#include "./Scene.h"
#include "Engine/Common/PoolManager.h"
#include "Engine/MiniEngine.h"

namespace Render
{
    /// <summary>
    /// ビューポートを作る設定データ
    /// </summary>
    struct ViewPortConfig
    {
        HE::Uint32 uSceneCount = 0;
        HE::Uint32 uWidth      = 0;
        HE::Uint32 uHeight     = 0;
    };

    /// <summary>
    /// ウィンドウのビューポート
    /// </summary>
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

}  // namespace Render
