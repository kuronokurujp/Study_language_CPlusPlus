#pragma once

// SDL2のフォントシステム
#include "Engine/Platform/PlatformFont.h"

namespace PlatformSDL2
{
    class PlatformSDL2Module;

    class Font final : public Platform::FontInterface
    {
    public:
        // このクラスはModule下にあるのでこのクラスが生きている間はModuleは必ず存在しているのを保障している
        Font(PlatformSDL2::PlatformSDL2Module*);

        void VRelease() override final;

        HE::Bool VLoad(const Platform::EFontSize in_eSizeType, const Core::File::Path& in_rTTFFilePath) override final;

        void VUnload() override final;

        Core::Memory::SharedPtr<class FontMaterial> GetMaterial() const { return this->_spMat; }
        Core::Memory::SharedPtr<class TextureSurface> GetTexture() const { return this->_spTex; }

    private:
        PlatformSDL2Module* _pModule = NULL;

        Core::Memory::SharedPtr<class FontMaterial> _spMat;
        Core::Memory::SharedPtr<class TextureSurface> _spTex;
    };
}  // namespace PlatformSDL2
