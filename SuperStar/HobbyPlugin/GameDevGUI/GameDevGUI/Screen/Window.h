#pragma once

#include "Engine/MiniEngine.h"
#include "Engine/Platform/PlatformScreen.h"

namespace GameDevGUI
{
    /// <summary>
    /// ウィンドウロジック
    /// </summary>
    class GameDevGUIWindowStrategy final : public Platform::WindowStrategy
    {
    public:
        GameDevGUIWindowStrategy(Core::Memory::UniquePtr<Platform::WindowStrategy>);

        void VBegin() override final;
        void VEnd() override final;
        void VUpdate(const HE::Float32 in_dt) override final;

        void VSetPos(const HE::Uint32 in_uX, const HE::Uint32 in_uY) override final;

        void VActive() override final;
        void VShow() override final;

        void VBeginRender() override final;
        void VEndRender() override final;

    private:
        Core::Memory::UniquePtr<Platform::WindowStrategy> _upSt;
        void* _pImGuiContext = NULL;
    };

}  // namespace GameDevGUI
