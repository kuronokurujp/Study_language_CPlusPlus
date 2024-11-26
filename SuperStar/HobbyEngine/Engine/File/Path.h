#pragma once

#include "Engine/Common/CustomString.h"
#include "Engine/Core.h"

// ファイルのパス制御ができる

namespace Core::File
{
    /// <summary>
    /// ファイルパス制御
    /// パスの連結とかできる
    /// </summary>
    class Path final
    {
        HE_CLASS_MOVE_NG(Path);

    public:
        Path() = default;

        /// <summary>
        /// パス文字列を一つのみ設定
        /// </summary>
        Path(const Char* in_szPath) { this->_Set(in_szPath); }
        Path(const Path& in_rPath) { this->_Set(in_rPath.Str()); }

        /// <summary>
        /// パス文字列を複数設定
        /// </summary>
        template <typename... TArgs>
        Path(TArgs... in_args)
        {
            // TODO: 未対応の型でエラーになった場合, 追跡ができない
            // これ非対応にして変わりのを用意する
            (this->_Append(in_args), ...);
        }

#ifdef HE_WIN
        Path(const UTF8* in_szPathUTF8) : _szPath(in_szPathUTF8) {}
#endif

        void operator=(const Path&);
        void operator=(const Char*);

        Path& operator+=(const Path&);

        inline const Char* Str() const { return this->_szPath.Str(); }
        inline Bool Empty() const { return this->_szPath.Empty(); }

    private:
        inline void _Set(const Char* in_szPath) HE_NOEXCEPT { this->_szPath = in_szPath; }
        void _Append(const Char* in_szPath);

    private:
        Core::Common::FixedString256 _szPath;
    };

}  // namespace Core::File
