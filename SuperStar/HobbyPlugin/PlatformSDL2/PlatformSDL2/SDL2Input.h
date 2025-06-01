#pragma once

#include "Engine/Common/PoolManager.h"

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

        void VUpdate(const HE::Float32) override final;

        // Inputオブジェクト
        // マウスやキーボードなどの入力情報はオブジェクトに含まれている
        virtual const Core::Common::Handle VCreateObject() override final;
        virtual void VReleaseObject(Core::Common::Handle&) override final;

        virtual Platform::InputObject* GetObj(const Core::Common::Handle) override final;

    private:
        Core::Common::RuntimePoolManager<Platform::InputObject> _poolInputObject;
    };
}  // namespace PlatformSDL2
