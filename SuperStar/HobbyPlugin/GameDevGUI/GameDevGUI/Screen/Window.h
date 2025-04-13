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
        void VRelease() override final;

        void VBegin() override final;
        void VEnd() override final;
        void VUpdate(const HE::Float32 in_dt) override final;

        void VSetPos(const HE::Uint32 in_uX, const HE::Uint32 in_uY) override final;

        void VActive() override final;
        void VShow() override final { this->_upSt->VShow(); }
        void VHide() override final { this->_upSt->VHide(); }

        void VBeginRender() override final;
        void VEndRender() override final;

        const HE::Bool VIsClose() const override final { return this->_upSt->VIsClose(); }

        void VRegistEventMenuCallback(EventMenuCallback in_callback) override final
        {
            this->_upSt->VRegistEventMenuCallback(std::move(in_callback));
        }

        const Core::Common::Handle VGetHandle() const override final
        {
            return this->_upSt->VGetHandle();
        }

        const Platform::WindowConfig& VGetConfig() const override final
        {
            return this->_upSt->VGetConfig();
        }

    private:
        Core::Memory::UniquePtr<Platform::WindowStrategy> _upSt;
        void* _pImGuiContext = NULL;
    };

}  // namespace GameDevGUI
