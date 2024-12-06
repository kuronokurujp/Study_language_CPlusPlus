#pragma once

// DXLib用の入力システム
#include "Engine/Platform/PlatformInput.h"

namespace PlatformSDL2
{
    /// <summary>
    /// DxLib用の入力システム
    /// </summary>
    class Input final : public Platform::InputInterface
    {
    public:
        void VInit() override final;
        void VUpdate(const HE::Float32 in_fDeltaTime) override final;
        Bool VIsQuit() const override final { return this->_bQuit; }

    private:
        Uint32 _uCurrButton = 0;
        Uint32 _uPrevButton = 0;
        Bool _bQuit         = FALSE;
    };
}  // namespace Platform::SDL2
