#pragma once

#include <string>

#include "Type.h"

// プラットフォームがWindows

#if !defined(HE_CHARACTER_CODE_UTF8) && defined(HE_WIN)
// プラットフォームのChar型がWChar型
#define HE_USE_WCHAR
#else
// プラットフォームのChar型がchar型
#endif

#ifdef HE_USE_WCHAR

#include <wchar.h>

#define HE_STR_LENGTH(t) static_cast<HE::Uint32>(::wcslen((t)))
#define HE_STR_CMP(a, b) ::wcscmp(a, b)

// 第二引数にはコピー配列の要素数を設定
// wcharの配列サイズだと要素数x2の設定でコピー処理をしてオーバーフローを起こした
// https://learn.microsoft.com/ja-jp/cpp/c-runtime-library/reference/strncpy-s-strncpy-s-l-wcsncpy-s-wcsncpy-s-l-mbsncpy-s-mbsncpy-s-l?view=msvc-170
#define HE_STR_COPY_S(dst, dst_len, src, src_len) ::wcsncpy_s(dst, (dst_len), src, (src_len))
#define HE_STR_STR(t01, t02) ::wcsstr(t01, t02)
#define HE_STR_VSNPRINTF(dst, len, count, fmt, arg) ::_vsnwprintf_s(dst, len, count, fmt, arg)
#define HE_STR_LOWER(s, size) ::_wcslwr_s(s, (size))
#define HE_STR_UPPER(s, size) ::_wcsupr_s(s, (size))

// wchar_t型を標準テキスト型に
#define HE_STR_TEXT(t) L##t
#define HE_STR_EMPTY L""
// 一文字の空文字
#define HE_STR_CHAR_EMPTY L'\0'

// 1文字を小文字に変える
#define HE_CHAR_LOWWER(c) ::towlower(c)

// プラットフォームがCLI
#else

#include <cctype>

#define HE_STR_LENGTH(t) static_cast<HE::Uint32>(::strlen(t))
#define HE_STR_CMP(a, b) ::strcmp(a, b)

#define HE_STR_COPY_S(dst, dst_size, src, src_len) ::strncpy_s(dst, dst_size, src, src_len)
#define HE_STR_STR(t01, t02) ::strstr(t01, t02)
#define HE_STR_VSNPRINTF(dst, len, count, fmt, arg) ::vsnprintf_s(dst, len, count, fmt, arg)

#define HE_STR_LOWER(s, size)                 \
    {                                         \
        for (HE::Uint32 i = 0; i < size; ++i) \
        {                                     \
            s[i] = ::tolower(s[i]);           \
        }                                     \
    }
#define HE_STR_UPPER(s, size)                 \
    {                                         \
        for (HE::Uint32 i = 0; i < size; ++i) \
        {                                     \
            s[i] = ::toupper(s[i]);           \
        }                                     \
    }

// HE::UTF8を標準テキスト型に
#define HE_STR_TEXT(t) u8##t
#define HE_STR_EMPTY u8""
// 一文字の空文字
#define HE_STR_CHAR_EMPTY u8'\0'

// 1文字を小文字に変える
#define HE_CHAR_LOWWER(c) ::tolower(c)

#endif

// HE::UTF8の文字列を記述する時のマクロ
// 必ずUTF8の文字列を使う時に利用
#define HE_STR_U8_TEXT(t) u8##t

// UTF8(char)専用のマクロ
// どんな環境で必ずUTF8を扱う時に利用
#define HE_STR_U8_LENGTH(t) static_cast<HE::Uint32>(::strlen(t))
#define HE_STR_U8_CMP(a, b) ::strcmp(a, b)

#define HE_STR_U8_COPY_S(dst, dst_size, src, src_len) ::strncpy_s(dst, dst_size, src, src_len)
#define HE_STR_U8_STR(t01, t02) ::strstr(t01, t02)
#define HE_STR_U8_VSNPRINTF(dst, len, count, fmt, arg) ::vsnprintf_s(dst, len, count, fmt, arg)

#define HE_STR_U8_LOWER(s, size)              \
    {                                         \
        for (HE::Uint32 i = 0; i < size; ++i) \
        {                                     \
            s[i] = ::tolower(s[i]);           \
        }                                     \
    }
#define HE_STR_U8_UPPER(s, size)              \
    {                                         \
        for (HE::Uint32 i = 0; i < size; ++i) \
        {                                     \
            s[i] = ::toupper(s[i]);           \
        }                                     \
    }

// WCharを固定で利用する場合の文字列マクロ
#if defined(HE_WIN)

#define HE_STR_W_TEXT(t) L##t

#else
#define HE_STR_W_TEXT(t) ##t

#endif


// 文字列の終端を表す
#define HE_STR_END_POINT HE_STR_TEXT('\0')

#define HE_STR_ERRNO errno_t
#define HE_STR_SUCCESS(e) e == 0 ? TRUE : FALSE
