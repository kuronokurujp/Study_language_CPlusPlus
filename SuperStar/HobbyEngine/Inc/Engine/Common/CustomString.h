#pragma once

#include <functional>
#include <string_view>

#include "CustomArray.h"
#include "Engine/Core.h"

namespace Core::Common
{
    /// <summary>
    /// 文字列制御の基本クラス
    /// </summary>
    class StringBase
    {
        HE_CLASS_DEFAULT_CONSTRUCT_NG(StringBase);
        HE_CLASS_MOVE_CONSTRUCT_NG(StringBase);

    public:
        // 文字列の文字送りするイテレーター
        class IteratorChar final
        {
        public:
            IteratorChar(HE::Char* in_pStr, const HE::Sint32 in_sIndex)
                : _pStr(in_pStr), _sIndex(in_sIndex)
            {
            }

            // インクリメント
            IteratorChar& operator++();

            // インクリメント
            IteratorChar operator++(int)
            {
                IteratorChar iter = *this;
                ++(*this);
                return iter;
            }

            // 比較
            bool operator==(const IteratorChar& in_rIter) const HE_NOEXCEPT
            {
                if (this->_sIndex == in_rIter._sIndex) return TRUE;

                return (in_rIter._sIndex <= this->_sIndex);
            }

            // 現在参照している文字
            HE::Char* operator*();

            // 比較
            bool operator!=(const IteratorChar& in_crIter) const { return !(*this == in_crIter); }

        private:
#if !defined(HE_CHARACTER_CODE_UTF8) && defined(HE_WIN)
            HE::Char _aChar[1] = {0};
#else

            HE::Char _aChar[7] = {0};
#endif
            HE::Char* _pStr    = NULL;
            HE::Sint32 _sIndex = 0;
        };

    public:
        /// <summary>
        /// in_bUseBuffがTRUNならバッファにはすでに文字がありそれを使いまわす
        /// </summary>
        StringBase(HE::Char* in_szBuff, const HE::Uint32 in_uCapacity,
                   const HE::Bool in_bUseBuff = FALSE);

        virtual ~StringBase() = default;
        StringBase& Replace(const HE::Char* in_szOld, const HE::Char* in_szNew);
        StringBase& Insert(const HE::Uint32 in_uIndex, const HE::Char* in_szInsert);
        StringBase& Remove(const HE::Uint32 in_uIndex, const HE::Uint32 in_uCount = 1);
        StringBase& Format(const HE::Char* in_cszFormat, ...);
        StringBase& FormatV(const HE::Char* in_szFormat, va_list in_vlist);
        void Clear() HE_NOEXCEPT { this->_szBuff[0] = '\0'; }

        // 文字列の先頭と末尾の文字を取得
        HE::Char FirstChar() const { return this->_szBuff[0]; }
        HE::Char LastChar() const
        {
            auto _uSize = static_cast<HE::Uint32>(HE_STR_LENGTH(this->_szBuff));
            return this->_szBuff[_uSize - 1];
        }

        /// <summary>
        /// 文字列を見つける
        /// 存在しない場合はSize関数の値を渡す
        /// </summary>
        HE::Sint32 Find(const HE::Char* in_szName, const HE::Uint32 in_uStart = 0) const;

        HE::Bool Empty() const HE_NOEXCEPT { return this->_szBuff[0] == '\0'; }

        // 文字列の文字容量数
        inline HE::Uint32 Capacity() const HE_NOEXCEPT { return this->_uCapacity; }

        // 文字列の文字数(利用しているバッファ数でない)
        HE::Uint32 Length() const;

        // 文字データの要素数(文字数ではない, UTF8だと要素数=文字数とはならない)
        inline HE::Uint32 Size() const HE_NOEXCEPT
        {
            return static_cast<HE::Uint32>(HE_STR_LENGTH(this->_szBuff));
        }

        inline const HE::Char* Str() const HE_NOEXCEPT { return this->_szBuff; }

        /// <summary>
        /// 文字列をハッシュ化して返す
        /// </summary>
        HE::Uint64 Hash() const;

        /// <summary>
        /// HE::UTF8として出力
        /// 文字列をUTF-8として利用したい場合に利用
        /// </summary>
        void OutputUTF8(HE::UTF8* out, const HE::Uint32 in_uLen) const;

        /// <summary>
        /// WCharの文字列を出力
        /// </summary>
        void OutputW(HE::WChar* out, const HE::Uint32 in_uLen) const;

        /// <summary>
        /// 文字列を出力
        /// </summary>
        void Output(HE::Char* out, const HE::Uint32 in_uLen) const;

        // 大文字 / 小文字にする
        void ToLower()
        {
            HE_STR_LOWER(this->_szBuff, HE_STR_LENGTH(this->_szBuff) * sizeof(HE::Char));
        }
        void ToUpper()
        {
            HE_STR_UPPER(this->_szBuff, HE_STR_LENGTH(this->_szBuff) * sizeof(HE::Char));
        }

        // 数値を文字列に変換
        void operator=(const HE::Uint32 in_uNum)
        {
#if !defined(HE_CHARACTER_CODE_UTF8) && defined(HE_WIN)
            ::swprintf(this->_szBuff, this->_uCapacity, L"%d", in_uNum);
#else
            ::snprintf(this->_szBuff, this->_uCapacity, "%d", in_uNum);
#endif
        }

        StringBase& operator=(const HE::Char* in_szName)
        {
            this->_Copy(in_szName, this->_uCapacity);
            return *this;
        }

        StringBase& operator=(const StringBase& r)
        {
            this->_Copy(r.Str(), this->_uCapacity);
            return *this;
        }

        StringBase& operator=(StringBase&& in_rr)
        {
            this->_Copy(in_rr.Str(), this->_uCapacity);
            return *this;
        }

        StringBase& operator=(const StringBase&& in_rr)
        {
            this->_Copy(in_rr.Str(), this->_uCapacity);
            return *this;
        }

        StringBase& operator+=(const HE::Char c)
        {
            this->_Add(c);
            return *this;
        }

        StringBase& operator+=(const HE::Char* in_szName)
        {
            this->_Add(in_szName);
            return *this;
        }

        StringBase& operator+=(const StringBase& r)
        {
            this->_Add(r.Str());
            return *this;
        }

        // いくつかの型を指定できるようにする
        template <typename... TArgs>
        typename std::enable_if<((std::is_same<TArgs, HE::Char*>::value ||
                                  std::is_same<TArgs, const HE::Char*>::value) &&
                                 ...),
                                void>::type
        Concatenate(TArgs... args)
        {
            // 引数の個数を取得
            const HE::Uint32 _uCount = static_cast<HE::Uint32>(sizeof...(args));
            if (_uCount <= 0) return;

            // 初期化リストを使用して引数を処理
            const HE::Char* carrValue[] = {args...};
            for (HE::Uint32 i = 0; i < _uCount; ++i) this->_Add(carrValue[i]);
        }

        /// <summary>
        /// 文字列の文字を一つずつ扱う先頭イテレーターを取得
        /// </summary>
        IteratorChar Begin() const HE_NOEXCEPT { return IteratorChar(this->_szBuff, 0); }

        /// <summary>
        /// 文字列の文字を一つずつ扱う終端イテレーター取得
        /// </summary>
        IteratorChar End() const HE_NOEXCEPT { return IteratorChar(this->_szBuff, this->Size()); }

        HE::Bool operator==(const HE::Char* in_szName) const
        {
            return (in_szName && (HE_STR_CMP(this->_szBuff, in_szName) == 0));
        }

        HE::Bool operator==(const StringBase& in_szrName) const
        {
            return operator==(in_szrName.Str());
        }
        HE::Bool operator!=(const HE::Char* in_szName) const
        {
            return (operator==(in_szName)) == FALSE;
        }
        HE::Bool operator!=(const StringBase& in_szrName) const
        {
            return (operator==(in_szrName)) == FALSE;
        }
        HE::Bool operator<(const HE::Char* in_szName) const
        {
            return HE_STR_CMP(this->_szBuff, in_szName) < 0;
        }
        HE::Bool operator<(const StringBase& in_szrName) const
        {
            return HE_STR_CMP(this->_szBuff, in_szrName.Str()) < 0;
        }
        HE::Bool operator<=(const HE::Char* in_szName) const
        {
            return HE_STR_CMP(this->_szBuff, in_szName) <= 0;
        }
        HE::Bool operator<=(const StringBase& in_szrName) const
        {
            return HE_STR_CMP(this->_szBuff, in_szrName.Str()) <= 0;
        }
        HE::Bool operator>(const HE::Char* in_szName) const
        {
            return HE_STR_CMP(this->_szBuff, in_szName) > 0;
        }
        HE::Bool operator>(const StringBase& in_szrName) const
        {
            return HE_STR_CMP(this->_szBuff, in_szrName.Str()) > 0;
        }
        HE::Bool operator>=(const HE::Char* in_szName) const
        {
            return HE_STR_CMP(this->_szBuff, in_szName) >= 0;
        }
        HE::Bool operator>=(const StringBase& in_szrName) const
        {
            return HE_STR_CMP(this->_szBuff, in_szrName.Str()) >= 0;
        }

        HE::Char operator[](const HE::Uint32 in_uCuIndex) const
        {
            if (this->_uCapacity <= in_uCuIndex) return '\0';

            return this->_szBuff[in_uCuIndex];
        }

    protected:
        StringBase& _Copy(const HE::Char* in_szName, const HE::Uint32 in_uLen);

    private:
        StringBase& _Add(const HE::Char*);
        StringBase& _Add(const HE::Char c);

    private:
        HE::Char* _szBuff     = NULL;
        HE::Uint32 _uCapacity = 0;
    };

    /// <summary>
    /// 固定長の文字列クラス
    /// </summary>
    template <HE::Uint32 TCapacity>
    class FixedString final : public StringBase
    {
    public:
        // 基本クラスのオペレーターを使えるようにしている
        using StringBase::operator=;

    public:
        FixedString() : StringBase(this->_szBuff, TCapacity) {}
        FixedString(const HE::Char* in_szName) : StringBase(this->_szBuff, TCapacity)
        {
            this->_Copy(in_szName, HE_STR_LENGTH(in_szName));
        }
#if !defined(HE_CHARACTER_CODE_UTF8) && defined(HE_WIN)
        FixedString(const HE::UTF8* in_szNameUTF8) : StringBase(this->_szBuff, TCapacity)
        {
            // Win版のみHE::UTF8型がchar型なので切り替える
            this->_ConvUTF8toWide(in_szNameUTF8, static_cast<HE::Uint32>(::strlen(in_szNameUTF8)));
        }
#endif

        FixedString(const FixedString<TCapacity>& r) : StringBase(this->_szBuff, TCapacity)
        {
            *this = r;
        }
        FixedString(const FixedString<TCapacity>&& r) : StringBase(this->_szBuff, TCapacity)
        {
            *this = r;
        }
        FixedString(const StringBase& r) : StringBase(this->_szBuff, TCapacity)
        {
            this->_Copy(r.Str(), r.Size());
        }

#if !defined(HE_CHARACTER_CODE_UTF8) && defined(HE_WIN)
        FixedString(const std::string_view& in_szrName) : StringBase(this->_szBuff, TCapacity)
        {
            this->_ConvUTF8toWide(in_szrName.data(), static_cast<HE::Uint32>(in_szrName.length()));
        }
#else
        FixedString(const std::string_view& in_szName) : StringBase(this->_szBuff, TCapacity)
        {
            this->_Copy(in_szName.data(), in_szName.length());
        }
#endif

        // win版では文字列はwchar / charの二つの型がある
        FixedString<TCapacity>& operator=(const HE::UTF8* in_szName)
        {
            if (in_szName)
            {
                // Win版のみHE::UTF8型がchar型なので切り替える
#if !defined(HE_CHARACTER_CODE_UTF8) && defined(HE_WIN)
                this->_ConvUTF8toWide(in_szName, TCapacity);
#else
                this->_Copy(in_szName, HE_STR_LENGTH(in_szName));
#endif
            }

            return *this;
        }

        FixedString<TCapacity>& operator=(const FixedString<TCapacity>& r)
        {
            this->_Copy(r._szBuff, HE_STR_LENGTH(r._szBuff));
            return *this;
        }

    private:
        /// <summary>
        /// HE::UTF8の文字列からワイドの文字列に変えて設定
        /// </summary>
        HE::Bool _ConvUTF8toWide(const HE::UTF8* in_szNameUTF8, const HE::Uint32 in_uLen)
        {
            // 作業用の変数は何度も使うので最初に作成しておく
            static HE::WChar w[TCapacity] = {};
            ::memset(w, 0, HE_ARRAY_SIZE(w));

            this->OutputW(w, in_uLen);
            // 出力した文字列をコピーする
            (*this) = w;

            return TRUE;
        }

    private:
        HE::Char _szBuff[TCapacity] = {};
    };

    // 固定長の文字列型
    typedef FixedString<16> FixedString16;
    typedef FixedString<32> FixedString32;
    typedef FixedString<64> FixedString64;
    typedef FixedString<128> FixedString128;
    typedef FixedString<256> FixedString256;
    typedef FixedString<512> FixedString512;
    typedef FixedString<1024> FixedString1024;

    /// <summary>
    /// UTF8文字列を区切る
    /// </summary>
    extern void SplitWithUTF8(std::function<void(const HE::UTF8*, const HE::Uint32)> in_resultFunc,
                              HE::UTF8* in_szName, const HE::Uint32 in_uLen,
                              const HE::UTF8* in_szDelim);

    /// <summary>
    /// 指定文字列を指定文字で区切って出力
    /// </summary>
    static void OutputSplitString(ArrayBase<FixedString1024>& out, StringBase& in_szrName,
                                  const HE::Char in_delim)
    {
        FixedString1024 name;
        HE::Uint32 i          = 0;
        HE::Uint32 arrayIndex = 0;
        HE::Char c            = in_szrName[i];
        while (c != '\0')
        {
            if (c == in_delim)
            {
                if (name.Empty() == FALSE)
                {
                    out.Set(arrayIndex, name);
                    ++arrayIndex;
                }
                name.Clear();
            }
            else
            {
                name += c;
            }
            ++i;
            c = in_szrName[i];
        }

        if (name.Empty() == FALSE)
        {
            out.Set(arrayIndex, name);
            ++arrayIndex;
        }
    }

    /// <summary>
    /// UTF8の文字からユニコード値にして返す
    /// </summary>
    extern HE::Uint32 GetUTF8CharToUnicode(const HE::UTF8*);

    // テンプレートクラス FixedString の部分的な型特性
    template <typename T>
    struct IsFixedString : std::false_type
    {
    };

    // FixedStringのインスタンスに対する特殊化
    template <HE::Uint32 TCapacity>
    struct IsFixedString<FixedString<TCapacity>> : std::true_type
    {
    };

    /// <summary>
    /// 作業用の文字列変数
    /// ローカル内で文字列制御をする一時利用できる変数
    /// ※グローバルで利用は絶対してはいけない
    /// </summary>
    extern FixedString16 g_szTempFixedString16;
    extern FixedString32 g_szTempFixedString32;
    extern FixedString64 g_szTempFixedString64;
    extern FixedString128 g_szTempFixedString128;
    extern FixedString256 g_szTempFixedString256;
    extern FixedString512 g_szTempFixedString512;
    extern FixedString1024 g_szTempFixedString1024;

    /// <summary>
    /// 作業用のUTF8文字列変数
    /// ローカル内で文字列制御をする一時利用できる変数
    /// ※グローバルで利用は絶対してはいけない
    /// </summary>
    extern HE::UTF8 g_szTempFixedUTF8_16[16];
    extern HE::UTF8 g_szTempFixedUTF8_32[32];
    extern HE::UTF8 g_szTempFixedUTF8_64[64];
    extern HE::UTF8 g_szTempFixedUTF8_128[128];
    extern HE::UTF8 g_szTempFixedUTF8_256[256];
    extern HE::UTF8 g_szTempFixedUTF8_512[512];
    extern HE::UTF8 g_szTempFixedUTF8_1024[1024];

}  // namespace Core::Common
