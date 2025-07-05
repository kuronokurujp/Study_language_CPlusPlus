#pragma once

// 利用頻度の多いマクロとテンプレート関数を用意
// マクロとして利用するのでテンプレート関数名はマクロの表記ルールにしている

#include <cmath>

#include "Config.h"
#include "Str.h"
#include "Type.h"

// UTF8文字列から数値に変換
#define HE_U8_TO_UINT32(__x__) std::stoul((__x__))
#define HE_U8_TO_FLOAT32(__x__) std::stof((__x__))
// 16進数UTF8文字列を数値に変換
#define HE_U8_HEX_TO_UINT32(__x__) std::stoul((__x__), NULL, 16)

// 引数に設定した32bit変数のバイト並びを反転して書き換え
#define HE_SWAP_BYTE_32BIT(__x__)                    \
    {                                                \
        HE::Uint32 uNewValue = 0;                    \
        uNewValue += (((__x__) & 0xff000000) >> 24); \
        uNewValue += (((__x__) & 0x00ff0000) >> 8);  \
        uNewValue += (((__x__) & 0x0000ff00) << 8);  \
        uNewValue += (((__x__) & 0x000000ff) << 24); \
                                                     \
        (__x__) = uNewValue;                         \
    }                                                \
    while (0)

// 配列の要素数
// 配列のポイントでは利用できない
#define HE_ARRAY_NUM(_array_) (sizeof((_array_)) / sizeof(((_array_)[0])))

// 配列の全データサイズ
// 配列のポイントでは利用できない
#define HE_ARRAY_SIZE(_tbl_) (sizeof(_tbl_))

// コンパイル時のアサート
#define HE_STATIC_ASSERT(...) static_assert(__VA_ARGS__)

// デフォルトコンストラクタを封印
#define HE_CLASS_DEFAULT_CONSTRUCT_NG(_x_) _x_() = delete;

// クラスのコピー封印
#define HE_CLASS_COPY_CONSTRUCT_NG(_x_) \
    _x_(_x_&)       = delete;           \
    _x_(const _x_&) = delete;

#define HE_CLASS_COPY_NG(_x_)            \
    _x_(_x_&)                  = delete; \
    _x_(const _x_&)            = delete; \
    _x_& operator=(_x_&)       = delete; \
    _x_& operator=(const _x_&) = delete;

// セマンティクスコンストラクターと演算子を封印
#define HE_CLASS_MOVE_CONSTRUCT_NG(_x_) \
    _x_(_x_&&)       = delete;          \
    _x_(const _x_&&) = delete;

#define HE_CLASS_MOVE_OPERATOR_NG(_x_)    \
    _x_& operator=(_x_&&)       = delete; \
    _x_& operator=(const _x_&&) = delete;

#define HE_CLASS_MOVE_NG(_x_)             \
    _x_(_x_&&)                  = delete; \
    _x_(const _x_&&)            = delete; \
    _x_& operator=(_x_&&)       = delete; \
    _x_& operator=(const _x_&&) = delete;

// 値のmin/maxマクロ
// 上限値の制御
#define HE_MAX(__src__, __max__) ((__src__) < (__max__) ? (__max__) : (__src__))
// 下限値の制御
#define HE_MIN(__src__, __min__) ((__src__) < (__min__) ? (__src__) : (__min__))

// 最大・最小の値でリピートした値を返すマクロ
// 浮動小数点数型の場合
template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type HE_LOOP_IN_RANGE(T t, T min,
                                                                                    T max)
{
    T length = max - min;
    T offset = t - min;
    T n      = std::floor(offset / length);
    return offset - n * length + min;
}

// 整数型の場合
template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type HE_LOOP_IN_RANGE(T t, T min, T max)
{
    T length = max - min + 1;
    T offset = t - min;
    T n      = offset / length;
    return offset - n * length + min;
}

// 値をmin/max内で丸めるマクロ
#define HE_CLAMP(__src__, __min__, __max__) \
    ((__src__) > (__max__) ? (__max__) : ((__src__) < (__min__) ? (__min__) : (__src__)))

// 例外を作らないキーワード
// クラスのメソッドで絶対に例外が起きない処理ではつける
// 例外をつけないコードをコンパイラが生成して最適化になる
// 処理が入らないプロパティのゲッターやセッターで使うのがいいと思う
#define HE_NOEXCEPT noexcept

// デバッグ専用処理
#ifdef HE_ENGINE_DEBUG

// プラットフォームがWindows専用
#ifdef HE_WIN
#include <Windows.h>

// ログ処理
#define HE_LOG_MSG_SIZE (2046)
#define HE_FILE __FILEW__

// 書式指定子の更新処理
template <typename T>
void HE_LOG_UPDATE_FORMAT_STRING(std::wstring& in_szFormat, size_t& in_rPos, const T& in_arg)
{
    using DecayedT = std::decay_t<T>;  // 修飾子を除去した型

    if constexpr (std::is_same_v<DecayedT, HE::WChar*> ||
                  std::is_same_v<DecayedT, const HE::WChar*>)
    {
        if (in_rPos != std::wstring::npos)
        {
            in_szFormat.replace(in_rPos, 2, L"%ls");
        }
    }
    else if constexpr (std::is_same_v<DecayedT, HE::UTF8*> ||
                       std::is_same_v<DecayedT, const HE::UTF8*>)
    {
        if (in_rPos != std::wstring::npos)
        {
            // ANSI文字は正常に出るが、日本語など2byte以上を使う文字では文字化けする
            in_szFormat.replace(in_rPos, 2, L"%hs");
        }
    }

    // 次の書式指定子を探す
    in_rPos = in_szFormat.find(L"%", in_rPos + 1);
}

// 共通処理を行う関数の作成
// 文字列書式で%sが使える
template <typename... TArgs>
HE::Bool HE_LOG_CREATE_FORMATERD_STRING(HE::WChar* out, const HE::Char* in_szFormat,
                                        TArgs... in_args)
{
#ifdef HE_CHARACTER_CODE_UTF8
    // static HE::WChar szFormat[HE_LOG_MSG_SIZE]    = {};
    static HE::UTF8 szUTF8Format[HE_LOG_MSG_SIZE] = {};

    int size = HE_MIN(std::snprintf(NULL, 0, in_szFormat, in_args...) + 1, HE_LOG_MSG_SIZE);

    std::snprintf(szUTF8Format, size, in_szFormat, in_args...);

    {
        // 利用する文字数を取得
        HE::Sint32 iUseSize =
            MultiByteToWideChar(CP_UTF8, 0, szUTF8Format, HE_LOG_MSG_SIZE, NULL, 0);
        // 利用する文字数が制限を超えていないかチェック
        if (iUseSize < 0)
        {
            // エラーが発生した場合は何もしない
            return FALSE;
        }

        // HE::UTF8文字列からUTF16の文字列に変える
        MultiByteToWideChar(CP_UTF8, 0, szUTF8Format, HE_LOG_MSG_SIZE, out, iUseSize);
    }
    // std::wstring szDynamicFormat = szFormat;
// #ifdef HE_CHARACTER_CODE_UTF8
#else
    std::wstring szDynamicFormat = in_szFormat;
    size_t pos                   = szDynamicFormat.find(L"%");
    // 各引数に応じてフォーマット文字列を変更する
    // フォーマット置換データが文字列のwchar_t型とchart型と両方使える
    ((HE_LOG_UPDATE_FORMAT_STRING(szDynamicFormat, pos, in_args)), ...);

    // 変換された引数を連結してワイド文字列を作成
    try
    {
        // 変換された引数を持つ動的なフォーマット文字列で出力する
        // TODO: 置換文字列の文字数が万になると例外エラーになる
        // TODO: 文字数が万など大きな数になった場合,例外エラーにならない対応が必要
        ::_snwprintf_s(out, HE_LOG_MSG_SIZE, HE_LOG_MSG_SIZE, szDynamicFormat.c_str(), in_args...);
    }
    catch (const std::exception& e)
    {
        return FALSE;
    }

#endif

    return TRUE;
}

// 文字列のローカル変数を利用するのでwhile文で囲っている
// ログ出力(改行なし)
// format引数は必ず文字列リテラルを設定する
// 文字列型の変数を入れるとコンパイルエラーになる
// コンソールにも出力
template <typename... TArgs>
void HE_LOG(const HE::Char* in_szFormat, TArgs... in_args)
{
    // 共通部分を関数で呼び出す

    static HE::WChar szText[HE_LOG_MSG_SIZE] = {};
    if (HE_LOG_CREATE_FORMATERD_STRING(szText, in_szFormat, in_args...) == FALSE)
    {
        // エラーが発生した場合は何もしない
        return;
    }

    // コンソールなどデバッグ画面で出力
    {
        ::OutputDebugString(szText);

        // WriteConsoleWを使ってワイド文字列を直接出力する
        HANDLE hConsole = ::GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole != INVALID_HANDLE_VALUE)
        {
            DWORD written = 0;
            ::WriteConsoleW(hConsole, szText, static_cast<DWORD>(::wcslen(szText)), &written,
                            nullptr);
        }
    }
}

// ログ出力(改行をする)
// format引数は必ず文字列リテラルを設定する
// 文字列型の変数を入れるとコンパイルエラーになる
// コンソールにも出力
template <typename... TArgs>
void HE_LOG_LINE(const HE::Char* in_szFormat, TArgs... in_args)
{
    static HE::WChar szText[HE_LOG_MSG_SIZE] = {};
    if (HE_LOG_CREATE_FORMATERD_STRING(szText, in_szFormat, in_args...) == FALSE)
    {
        // エラーが発生した場合は何もしない
        return;
    }

    // コンソールなどデバッグ画面で出力
    {
        ::OutputDebugString(szText);
        ::OutputDebugString(L"\n");

        // WriteConsoleWを使ってワイド文字列を直接出力する
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole != INVALID_HANDLE_VALUE)
        {
            DWORD written = 0;
            ::WriteConsoleW(hConsole, szText, static_cast<DWORD>(::wcslen(szText)), &written,
                            nullptr);
            ::WriteConsoleW(hConsole, L"\n", 1, &written, nullptr);
        }
    }
}

// プログラムが把握する情報を付与したログ出力
// ファイルパスが長く長文になる可能性があるのでログサイズ2倍の文字列サイズを確保
// コンソールにも出力
#define HE_PG_LOG_LINE(format, ...)                                                             \
    do                                                                                          \
    {                                                                                           \
        static HE::WChar c[HE_LOG_MSG_SIZE] = {};                                               \
        HE_LOG_CREATE_FORMATERD_STRING(c, format, __VA_ARGS__);                                 \
        static HE::WChar c2[HE_LOG_MSG_SIZE * 2] = {};                                          \
        _snwprintf_s(c2, HE_LOG_MSG_SIZE * 2, HE_LOG_MSG_SIZE * 2, L"%ls:%d %ls", __FILEW__,    \
                     __LINE__, c);                                                              \
        OutputDebugString(c2);                                                                  \
        OutputDebugString(L"\n");                                                               \
        static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);                               \
        if (hConsole != INVALID_HANDLE_VALUE)                                                   \
        {                                                                                       \
            DWORD written = 0;                                                                  \
            ::WriteConsoleW(hConsole, c2, static_cast<DWORD>(::wcslen(c2)), &written, nullptr); \
            ::WriteConsoleW(hConsole, HE_STR_TEXT("\n"), 1, &written, nullptr);                 \
        }                                                                                       \
    } while (0)

// #ifdef HE_WIN
#else
// TODO: win以外の対応は不十分

#define HE_FILE __FILE__
#define HE_LOG_MSG_SIZE (2046)

// ログ出力(改行なし)
#define HE_LOG(s, ...) printf(s, __VA_ARGS__)
// ログ出力(改行をする)
#define HE_LOG_LINE(s, ...) \
    printf(s, __VA_ARGS__); \
    printf("\n")

// プログラムが把握する情報を付与したログ出力
// ファイルパスが長く長文になる可能性があるのでログサイズ2倍の文字列サイズを確保
#define HE_PG_LOG_LINE(format, ...)                                                                \
    do                                                                                             \
    {                                                                                              \
        static HE::Char c[HE_LOG_MSG_SIZE] = {};                                                   \
        _snprintf_s(c, HE_LOG_MSG_SIZE, HE_LOG_MSG_SIZE, format, __VA_ARGS__);                     \
        HE::Char c2[HE_LOG_MSG_SIZE * 2] = {};                                                     \
        _snprintf_s(c2, HE_LOG_MSG_SIZE * 2, HE_LOG_MSG_SIZE * 2, "%s:%d %s\n", HE_FILE, __LINE__, \
                    c);                                                                            \
        printf(c2);                                                                                \
    } while (0)

#endif

// アサート処理
#include <cassert>

// アサートマクロ
#define HE_ASSERT(...)                                     \
    do                                                     \
    {                                                      \
        if (!(__VA_ARGS__))                                \
        {                                                  \
            HE_PG_LOG_LINE(HE_STR_TEXT("Assert: %s"),      \
                           HE_STR_TEXT(#__VA_ARGS__));     \
            assert(__VA_ARGS__);                           \
        }                                                  \
    } while (0)

// 条件が偽の場合に return するアサートマクロ
#define HE_ASSERT_RETURN(...)                              \
    {                                                      \
        if (!(__VA_ARGS__))                                \
        {                                                  \
            HE_PG_LOG_LINE(HE_STR_TEXT("Assert: %s"),      \
                           HE_STR_TEXT(#__VA_ARGS__));     \
            assert(__VA_ARGS__);                           \
            return;                                        \
        }                                                  \
    }

// 条件が偽の場合に指定値を返すアサートマクロ
#define HE_ASSERT_RETURN_VALUE(_x_, ...)                   \
    {                                                      \
        if (!(__VA_ARGS__))                                \
        {                                                  \
            HE_PG_LOG_LINE(HE_STR_TEXT("Assert: %s"),      \
                           HE_STR_TEXT(#__VA_ARGS__));     \
            assert(__VA_ARGS__);                           \
            return (_x_);                                  \
        }                                                  \
    }

// #ifdef HE_ENGINE_DEBUG
// リリース版ではデバッグ命令はすべて空にする
#else

// ログ処理
// 書式指定子の更新処理
template <typename T>
void HE_LOG_UPDATE_FORMAT_STRING(std::wstring& in_szFormat, size_t& in_rPos, const T& in_arg)
{
}
// 共通処理を行う関数の作成
// 文字列書式で%sが使える
template <typename... TArgs>
HE::Bool HE_LOG_CREATE_FORMATERD_STRING(HE::WChar* out, const HE::Char* in_szFormat,
                                        TArgs... in_args)
{
}
// 文字列のローカル変数を利用するのでwhile文で囲っている
// ログ出力(改行なし)
// format引数は必ず文字列リテラルを設定する
// 文字列型の変数を入れるとコンパイルエラーになる
// コンソールにも出力
template <typename... TArgs>
void HE_LOG(const HE::Char* in_szFormat, TArgs... in_args)
{
}

// ログ出力(改行をする)
// format引数は必ず文字列リテラルを設定する
// 文字列型の変数を入れるとコンパイルエラーになる
// コンソールにも出力
template <typename... TArgs>
void HE_LOG_LINE(const HE::Char* in_szFormat, TArgs... in_args)
{
}

// プログラムが把握する情報を付与したログ出力
// ファイルパスが長く長文になる可能性があるのでログサイズ2倍の文字列サイズを確保
// コンソールにも出力
#define HE_PG_LOG_LINE(format, ...) ((void)0)

// アサート処理
#define HE_ASSERT(...) ((void)0)

#endif
