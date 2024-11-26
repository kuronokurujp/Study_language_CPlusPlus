#pragma once

#include "CustomArray.h"
#include "Engine/Core.h"

#ifdef HE_WIN

#include <string_view>

#endif

namespace Core::Common
{
    /// <summary>
    /// 文字列制御の基本クラス
    /// </summary>
    class StringBase
    {
        HE_CLASS_DEFAULT_CONSTRUCT_NG(StringBase);
        HE_CLASS_MOVE_NG(StringBase);

    public:
        StringBase(Char* in_szBuff, const Uint32 in_uCapacity);

        virtual ~StringBase() { this->Clear(); }
        StringBase& Replace(const Char* in_szOld, const Char* in_szNew);
        StringBase& Insert(const Uint32 in_uIndex, const Char* in_szInsert);
        StringBase& Remove(const Uint32 in_uIndex, const Uint32 in_uCount = 1);
        StringBase& Format(const Char* in_cszFormat, ...);
        StringBase& FormatV(const Char* in_szFormat, va_list in_vlist);
        void Clear() HE_NOEXCEPT { this->_szBuff[0] = '\0'; }

        Sint32 Find(const Char* in_szName, const Uint32 in_uStart = 0) const;

        Bool Empty() const HE_NOEXCEPT { return this->_szBuff[0] == '\0'; }

        // 文字列の文字容量数
        inline Uint32 Capacity() const HE_NOEXCEPT { return this->_uCapacity; }

        // 文字列の文字数
        // ワイド文字型だと配列の要素数=文字数とは限らないのでSizeメソッドとは別途文字数を取得するメソッドを用意
        Uint32 Length() const;

        inline const Char* Str() const HE_NOEXCEPT { return this->_szBuff; }

        /// <summary>
        /// 文字列をハッシュ化して返す
        /// </summary>
        Uint64 Hash() const;

        /// <summary>
        /// UTF8として出力
        /// 文字列をUTF-8として利用したい場合に利用
        /// </summary>
        void OutputUTF8(UTF8* out, const Uint32 in_uLen) const;

        // 大文字 / 小文字にする
        void ToLower() { HE_STR_LOWER(this->_szBuff, HE_STR_LEN(this->_szBuff) * sizeof(Char)); }
        void ToUpper() { HE_STR_UPPER(this->_szBuff, HE_STR_LEN(this->_szBuff) * sizeof(Char)); }

        // 数値を文字列に変換
        void operator=(const Uint32 in_uNum)
        {
#ifdef HE_WIN
            ::swprintf(this->_szBuff, this->_uCapacity, L"%d", in_uNum);
#else
            ::snprintf(this->_szBuff, this->_uCapacity, "%d", in_uNum);
#endif
        }

        StringBase& operator=(const Char* in_szName)
        {
            this->_Copy(in_szName, this->_uCapacity);
            return *this;
        }

        StringBase& operator=(const StringBase& r)
        {
            this->_Copy(r.Str(), this->_uCapacity);
            return *this;
        }

        StringBase& operator+=(const Char c)
        {
            this->_Add(c);
            return *this;
        }

        StringBase& operator+=(const Char* in_szName)
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
        typename std::enable_if<
            ((std::is_same<TArgs, Char*>::value || std::is_same<TArgs, const Char*>::value) && ...),
            void>::type
        Concatenate(TArgs... args)
        {
            // 引数の個数を取得
            const Uint32 uCount = static_cast<Uint32>(sizeof...(args));
            if (uCount <= 0) return;

            // 初期化リストを使用して引数を処理
            const Char* carrValue[] = {args...};
            for (Uint32 i = 0; i < uCount; ++i) this->_Add(carrValue[i]);
        }

        Bool operator==(const Char* in_szName) const
        {
            return (in_szName && (HE_STR_CMP(this->_szBuff, in_szName) == 0));
        }

        Bool operator==(const StringBase& in_szrName) const { return operator==(in_szrName.Str()); }
        Bool operator!=(const Char* in_szName) const { return (operator==(in_szName)) == FALSE; }
        Bool operator!=(const StringBase& in_szrName) const
        {
            return (operator==(in_szrName)) == FALSE;
        }
        Bool operator<(const Char* in_szName) const
        {
            return HE_STR_CMP(this->_szBuff, in_szName) < 0;
        }
        Bool operator<(const StringBase& in_szrName) const
        {
            return HE_STR_CMP(this->_szBuff, in_szrName.Str()) < 0;
        }
        Bool operator<=(const Char* in_szName) const
        {
            return HE_STR_CMP(this->_szBuff, in_szName) <= 0;
        }
        Bool operator<=(const StringBase& in_szrName) const
        {
            return HE_STR_CMP(this->_szBuff, in_szrName.Str()) <= 0;
        }
        Bool operator>(const Char* in_szName) const
        {
            return HE_STR_CMP(this->_szBuff, in_szName) > 0;
        }
        Bool operator>(const StringBase& in_szrName) const
        {
            return HE_STR_CMP(this->_szBuff, in_szrName.Str()) > 0;
        }
        Bool operator>=(const Char* in_szName) const
        {
            return HE_STR_CMP(this->_szBuff, in_szName) >= 0;
        }
        Bool operator>=(const StringBase& in_szrName) const
        {
            return HE_STR_CMP(this->_szBuff, in_szrName.Str()) >= 0;
        }

        Char operator[](const Uint32 in_uCuIndex) const
        {
            if (this->_uCapacity <= in_uCuIndex) return '\0';

            return this->_szBuff[in_uCuIndex];
        }

    protected:
        StringBase& _Copy(const Char* in_szName, const Uint32 in_uLen);

    private:
        StringBase& _Add(const Char*);
        StringBase& _Add(const Char c);

    private:
        Char* _szBuff     = NULL;
        Uint32 _uCapacity = 0;
    };

    /// <summary>
    /// 固定長の文字列クラス
    /// </summary>
    template <Uint32 TCapacity>
    class FixedString final : public StringBase
    {
    public:
        // 基本クラスのオペレーターを使えるようにしている
        using StringBase::operator=;

    public:
        FixedString() : StringBase(this->_szBuff, TCapacity) {}
        FixedString(const Char* in_szName) : StringBase(this->_szBuff, TCapacity)
        {
            this->_Copy(in_szName, HE_STR_LEN(in_szName));
        }
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
            this->_Copy(r.Str(), r.Length());
        }

        FixedString(const UTF8* in_szNameUTF8) : StringBase(this->_szBuff, TCapacity)
        {
            // Win版のみUTF8型がchar型なので切り替える
#ifdef HE_WIN
            this->_ConvUTF8toWide(in_szNameUTF8, static_cast<Uint32>(::strlen(in_szNameUTF8)));
#else
            this->_Copy(in_szNameUTF8, HE_STR_LEN(in_szNameUTF8));
#endif
        }

#ifdef HE_WIN
        FixedString(const std::string_view& in_szrName) : StringBase(this->_szBuff, TCapacity)
        {
            this->_ConvUTF8toWide(in_szrName.data(), static_cast<Uint32>(in_szrName.length()));
        }
#else
        FixString(const std::string_view& in_rStr) : FixStringBase(this->_caBuff, CAPACITY)
        {
            // TODO: 対応が必要
            HE_ASSERT(0);
        }
#endif

        // win版では文字列はwchar / charの二つの型がある
        FixedString<TCapacity>& operator=(const UTF8* in_szName)
        {
            // Win版のみUTF8型がchar型なので切り替える
#ifdef HE_WIN
            this->_ConvUTF8toWide(in_szName, TCapacity);
#else
            this->_Copy(in_szName, HE_STR_LEN(in_szName));
#endif
            return *this;
        }

        FixedString<TCapacity>& operator=(const FixedString<TCapacity>& r)
        {
            this->_Copy(r._szBuff, HE_STR_LEN(r._szBuff));
            return *this;
        }

    private:
#ifdef HE_WIN
        /// <summary>
        /// UTF8の文字列からワイドの文字列に変えて設定
        /// </summary>
        /// <param name="in_pStr"></param>
        /// <param name="in_len"></param>
        Bool _ConvUTF8toWide(const UTF8* in_szNameUTF8, const Uint32 in_uLen)
        {
            HE_ASSERT(in_szNameUTF8);
            HE_ASSERT(in_uLen <= TCapacity);

            static Char w[TCapacity] = {};
            ::memset(w, 0, HE_ARRAY_SIZE(w));

            // 利用する文字数を取得
            Sint32 iUseSize = MultiByteToWideChar(CP_UTF8, 0, in_szNameUTF8, in_uLen, NULL, 0);
            // 利用する文字数が制限を超えていないかチェック
            HE_ASSERT(iUseSize < TCapacity);

            // UTF8文字列からUTF16の文字列に変える
            MultiByteToWideChar(CP_UTF8, 0, in_szNameUTF8, TCapacity, &w[0], iUseSize);

            (*this) = w;

            return TRUE;
        }
#endif
    private:
        Char _szBuff[TCapacity] = {};
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
    /// 指定文字列を指定文字で区切って出力
    /// </summary>
    static void OutputSplitString(ArrayBase<FixedString1024>& out, StringBase& in_szrName,
                                  const Char in_delim)
    {
        FixedString1024 name;
        Uint32 i          = 0;
        Uint32 arrayIndex = 0;
        Char c            = in_szrName[i];
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

    // テンプレートクラス FixedString の部分的な型特性
    template <typename T>
    struct IsFixedString : std::false_type
    {
    };

    // FixedStringのインスタンスに対する特殊化
    template <Uint32 TCapacity>
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

}  // namespace Core::Common
