#pragma once

#include "Engine/File/Path.h"
#include "Engine/MiniEngine.h"

// プラットフォームのフォントシステム定義

namespace Platform
{
    class FontInterface
    {
    public:
        virtual ~FontInterface() = default;

        virtual void VRelease() = 0;

        virtual HE::Bool VLoad(const HE::Uint32 in_uFontSize,
                               std::initializer_list<Core::File::Path>) = 0;
        virtual void VUnload()                                          = 0;
    };
}  // namespace Platform
