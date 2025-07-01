#pragma once

// SDL2のフォントシステム
#include <functional>

#include "Engine/Platform/PlatformFont.h"
#include "PlatformSDL2/SDL2File.h"

namespace PlatformSDL2
{
    class PlatformSDL2Module;

    class Font final : public Platform::FontInterface
    {
        HE_GENERATED_CLASS_BODY_HEADER(Font, Platform::FontInterface);

    private:
        using FileBinaryDataFunc = std::function<File::BinaryData(const Core::File::Path&)>;

    public:
        /// <summary>
        /// 指定したファイルのバイナリデータをロードする関数を受け取る
        /// </summary>
        /// <param name=""></param>
        Font(FileBinaryDataFunc);

        void VRelease() override final;

        HE::Bool VLoad(const Platform::EFontSize in_eSizeType,
                       const Core::File::Path& in_rTTFFilePath) override final;

        void VUnload() override final;

        Core::Memory::SharedPtr<class FontMaterial> GetMaterial() const { return this->_spMat; }
        Core::Memory::SharedPtr<class TextureSurface> GetTexture() const { return this->_spTex; }

    private:
        FileBinaryDataFunc _fileBinaryDataFunc = NULL;

        Core::Memory::SharedPtr<class FontMaterial> _spMat;
        Core::Memory::SharedPtr<class TextureSurface> _spTex;
    };
}  // namespace PlatformSDL2
