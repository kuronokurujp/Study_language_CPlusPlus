#pragma once

// Windows環境かどうか
#if defined(_WIN64) || defined(_WIN86)

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

// x64 / x84でのポインタを整数へキャストする型
#ifdef HE_X64
using Ptr = unsigned long long;
#else
using Ptr = unsigned int;
#endif

using Int    = int;
using Sint32 = signed long;
using Sint64 = signed long long;
using Uint32 = unsigned long;
using Uint64 = unsigned long long;
using Sint16 = signed short;
using Uint16 = unsigned short;
using Sint8  = signed char;
using Uint8  = unsigned char;
// WindowsではUnicode前提実装なのでwchar型にしている
#ifdef HE_WIN
using Char = wchar_t;
using UTF8 = char;
#else
using Char = char;
using UTF8 = char;
#endif

using Bool    = char;
using Float32 = float;
using Float64 = double;

// 無効値
constexpr Uint32 uInvalidUint32  = 0xffffffff;
constexpr Uint64 ulInvalidUint64 = 0xffffffffffffffff;

// 定数定義
#ifndef FALSE

#define FALSE (0)

#endif

#ifndef TRUE

#define TRUE (1)

#endif

// ポインタのNULLなのでc++でのポインタNULLのnullptrに置き換える必要がある
// NULLが整数0だポインタのNULLとc++コンパイラは解釈してくれない
#if NULL

#undef NULL

#endif

#ifndef NULL

#define NULL (nullptr)

#endif  // NULL
