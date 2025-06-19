#pragma once

#include "Engine/Common/CustomString.h"
#include "Engine/Core.h"

// ファイルのパス制御ができる

namespace Core::File
{
    // カスタム型制約
    template <typename T>
    struct TemplateIsPathItem : std::disjunction<std::is_same<T, Core::Common::StringBase>,
                                                 std::is_same<T, const HE::Char*>,
                                                 std::is_convertible<T, Core::Common::StringBase> >
    {
    };

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
        Path(const HE::Char* in_szPath) { this->_Set(in_szPath); }
#if !defined(HE_CHARACTER_CODE_UTF8) && defined(HE_WIN)
        Path(const HE::UTF8* in_szPathUTF8) : _szPath(in_szPathUTF8) {}
#endif

        Path(const Path& in_rPath) { this->_Set(in_rPath.Str()); }

        /// <summary>
        /// パス文字列を複数設定
        /// </summary>
        template <typename... TArgs>
        Path(TArgs... in_TArgs)
        {
            // TODO: 未対応の型でエラーになった場合, 追跡ができない
            // これ非対応にして変わりのを用意する

            // 型チェック
            HE_STATIC_ASSERT((std::conjunction_v<TemplateIsPathItem<TArgs>...>), "");

            // 要素を処理
            (
                [this](const auto& arg)
                {
                    Path::s_szTempFixedString1024 = arg;
                    this->_Append(Path::s_szTempFixedString1024);
                }(in_TArgs),
                ...);

            //(this->_Append(in_TArgs), ...);
        }

        void operator=(const Path&);
        void operator=(const HE::Char*);

        Path& operator+=(const Path&);

        inline const HE::Char* Str() const { return this->_szPath.Str(); }
        inline HE::Bool Empty() const { return this->_szPath.Empty(); }

    private:
        inline void _Set(const HE::Char* in_szPath) HE_NOEXCEPT { this->_szPath = in_szPath; }
        void _Append(const Core::Common::StringBase& in_szPath);

    private:
        static inline Core::Common::FixedString1024 s_szTempFixedString1024;

        Core::Common::FixedString256 _szPath;
    };

}  // namespace Core::File
