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
        Input();
        void VRelease() override final;

        HE::Bool VUpdate(const HE::Float32 in_fDeltaTime) override final;

        virtual void SetInputEventCallback(OnInputCallback);

    private:
        HE::Uint32 _uCurrButton = 0;
        HE::Uint32 _uPrevButton = 0;

        OnInputCallback _onInputCallback = NULL;
    };
}  // namespace PlatformSDL2
