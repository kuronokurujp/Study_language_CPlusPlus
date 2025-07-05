#pragma once

#include "Engine/MiniEngine.h"
#include "Engine/Platform/Screen/Render.h"

namespace Platform
{
    struct SceneConfig
    {
    public:
        inline HE::Float32 GetWidthHalf() const HE_NOEXCEPT
        {
            return static_cast<HE::Float32>(_uWidth * 0.5f);
        }

        inline HE::Float32 GetHeightHalf() const HE_NOEXCEPT
        {
            return static_cast<HE::Float32>(_uHeight * 0.5f);
        }

        HE::Uint32 _uWidth  = 0;
        HE::Uint32 _uHeight = 0;
    };

    /// <summary>
    /// シーンの基本ロジック
    /// </summary>
    class SceneStrategy
    {
    public:
        using EventRender =
            std::function<void(Core::Memory::SharedPtr<RenderInterface>, const SceneConfig&)>;

    public:
        SceneStrategy(const SceneConfig&, Core::Memory::SharedPtr<RenderInterface>, EventRender);
        virtual ~SceneStrategy() = default;

        virtual HE::Bool VBegin() = 0;
        virtual void VEnd()       = 0;

        virtual void VRender() = 0;

        Core::Memory::SharedPtr<RenderInterface> GetRender() { return _spRender; }

    protected:
        SceneConfig _config;
        Core::Memory::SharedPtr<RenderInterface> _spRender;
        EventRender _eventRender;
    };

}  // namespace Platform
