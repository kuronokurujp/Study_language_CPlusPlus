#pragma once

#include "Engine/MiniEngine.h"
#include "Engine/Platform/PlatformScreen.h"

namespace GameDevGUI
{
    /// <summary>
    /// ImGUI用のシーン
    /// </summary>
    class GameDevGUISceneStrategy : public Platform::SceneStrategyInterface
    {
    public:
        GameDevGUISceneStrategy(Core::Memory::UniquePtr<Platform::SceneStrategyInterface>);
        HE::Bool VBegin() override;

        void VEnd() override final;

        void VUpdate(const HE::Float32) override final;

        void VBeginRender() override final;
        void VEndRender() override final;

    private:
        Core::Memory::UniquePtr<Platform::SceneStrategyInterface> _upSt;
    };

}  // namespace GameDevGUI
