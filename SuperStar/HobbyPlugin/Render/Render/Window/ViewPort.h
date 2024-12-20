#pragma once

#include "./Scene.h"
#include "Engine/Common/PoolManager.h"
#include "Engine/MiniEngine.h"

namespace Platform
{
    class ViewPortStrategy;
};

namespace Render
{
    /// <summary>
    /// ウィンドウのビューポート
    /// </summary>
    class ViewPort
    {
        friend class Window;

    public:
        HE::Bool Init(Core::Memory::UniquePtr<Platform::ViewPortStrategy>,
                      const HE::Uint32 in_uSceneCount);
        void Release();

        std::tuple<Core::Common::Handle, SceneViewBase*> AddSceneView(
            Core::Memory::UniquePtr<class Platform::SceneStrategyInterface> in_upStorategy);
        void RemoveScene(Core::Common::Handle&);

        SceneViewBase* GetScene(const Core::Common::Handle&);

        const HE::Uint32 Width() const;
        const HE::Uint32 Height() const;

    private:
        void _Begin();
        void _End();

        void _BeginRender();
        void _EndRender();

    private:
        Core::Common::RuntimePoolManager<SceneViewBase> _poolSceneManager;
        Core::Memory::UniquePtr<Platform::ViewPortStrategy> _upStrategy;
    };

}  // namespace Render
