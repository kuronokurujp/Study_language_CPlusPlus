#pragma once

// SDL2の入力システム
#include "Engine/Platform/PlatformInput.h"

namespace PlatformSDL2
{
    /// <summary>
    /// 入力システム
    /// </summary>
    class Input final : public Platform::InputInterface
    {
    public:
        void VInit() override final;
        void VUpdate(const HE::Float32 in_fDeltaTime) override final;
        HE::Bool VIsQuit() const override final { return this->_bQuit; }

    private:
        HE::Uint32 _uCurrButton = 0;
        HE::Uint32 _uPrevButton = 0;
        HE::Bool _bQuit         = FALSE;
    };
}  // namespace PlatformSDL2
