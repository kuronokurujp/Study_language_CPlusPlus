#pragma once

#include <cstdint>

#if __cplusplus >= 202002L
// C++20以降のコード
#define HE_CPP_20_OR_LATER 1
#elif __cplusplus >= 201703L
// C++17以降のコード
#define HE_CPP_17_OR_LATER 1
#else
// C++17未満のコード
#define HE_CPP_17_BEFORE 1
#endif

// Windows環境かどうか
#if defined(_WIN64) || defined(_WIN86)

// Windowsはリトルエンディアン
#define HE_LITTLE_ENDIAN

// コンソールかどうか
#ifdef _WINDOWS
#define HE_WIN
#else
#define HE_CLS
#endif

// アプリがx64(8byte)かx84(4byte)かを定義
#ifdef _WIN64

#define HE_X64

#else

#define HE_X84

#endif

#endif

namespace HE
{
    // x64 / x84でのポインタの型が変わる
    using Ptr = std::uintptr_t;

    using Int    = int;
    using Sint32 = signed long;
    using Sint64 = signed long long;
    using Uint32 = unsigned long;
    using Uint64 = unsigned long long;
    using Sint16 = signed short;
    using Uint16 = unsigned short;
    using Sint8  = signed char;
    using Uint8  = unsigned char;
    // TODO: ハッシュ用の型を用意
    // プラットフォームによって32/64bitと切り替える可能性を考えて
    using Hash = Uint64;

// WindowsではUnicode前提実装なのでwchar型にしている
#if !defined(HE_CHARACTER_CODE_UTF8) && defined(HE_WIN)
    using Char  = wchar_t;
    using WChar = wchar_t;
    using UTF8  = char;
#else

#ifdef HE_CPP_20_OR_LATER
    using Char = char8_t;
#else
    using Char = char;
#endif
    using UTF8  = char;
    using WChar = wchar_t;
#endif

    using Bool    = char;
    using Float32 = float;
    using Float64 = double;

    // 無効値
    constexpr Uint32 uInvalidUint32  = 0xffffffff;
    constexpr Uint64 ulInvalidUint64 = 0xffffffffffffffff;
}  // namespace HE

// 定数定義
#ifndef FALSE

#define FALSE (0)

#endif

#ifndef TRUE

#define TRUE (1)

#endif

// ポインタのNULLなのでc++でのポインタNULLのnullptrに置き換える必要がある
// NULLが整数0だとポインタのNULLとc++コンパイラは解釈してくれない
#ifdef NULL

#undef NULL

#endif

#ifndef NULL

#define NULL (nullptr)

#endif  // NULL
