#pragma once

#include "Engine/File/Path.h"
#include "Engine/MiniEngine.h"

// プラットフォームのフォントシステム定義

namespace Platform
{
    enum EFontSize
    {
        EFontSize_64 = 0,
    };

    class FontInterface
    {
        HE_GENERATED_CLASS_BASE_BODY_HEADER(FontInterface);

    public:
        virtual ~FontInterface() = default;

        virtual void VRelease() = 0;

        virtual HE::Bool VLoad(const EFontSize in_eSizeType,
                               const Core::File::Path& in_rTTFFilePath) = 0;
        virtual void VUnload()                                          = 0;
    };
}  // namespace Platform
