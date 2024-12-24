#pragma once

#include "Engine/Core.h"

namespace Core::Math
{
    struct Color32
    {
        HE::Uint8 r, g, b, a;
    };

    /// <summary>
    /// 色情報
    /// </summary>
    union Color
    {
        HE::Uint32 c;
        Color32 c32;
    };

    // 色のデータ一式
    namespace RGB
    {
        // エンディアンによってバイト並びが変わるので切り替えている
#ifdef HE_LITTLE_ENDIAN
        constexpr Color White{0xffffffff};
        constexpr Color Black{0xff000000};
        constexpr Color Red{0xff0000ff};
        constexpr Color Blue{0xffff0000};
        constexpr Color Green{0xff00ff00};
        constexpr Color Yellow{0xff00ffff};
        constexpr Color Cyan{0xffffff00};
        constexpr Color Magenta{0xffff00ff};
        constexpr Color Gray{0xff808080};
        constexpr Color Orange{0xffffa5ff};
#else
        constexpr Color White{0xffffffff};
        constexpr Color Black{0x000000ff};
        constexpr Color Red{0xff0000ff};
        constexpr Color Blue{0x0000ffff};
        constexpr Color Green{0x00ff00ff};
        constexpr Color Yellow{0xffff00ff};
        constexpr Color Cyan{0x00ffffff};
        constexpr Color Magenta{0xff00ffff};
        constexpr Color Gray{0x808080ff};
        constexpr Color Orange{0xffa5ff};
#endif
    };  // namespace RGB

}  // namespace Core::Math
