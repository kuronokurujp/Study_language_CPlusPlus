#include "Engine/Common/CustomString.h"

#include "Engine/Common/Hash.h"
#include "Engine/Memory/Memory.h"

namespace Core::Common
{
    namespace Local
    {
#if !defined(HE_CHARACTER_CODE_UTF8) && defined(HE_WIN)
#else
        static HE::Uint32 GetUTF8CharOffset(const HE::Uint8 in_char)
        {
            if (in_char == 0x00) return 0;

            // UTF8のコード表を見ると1byteが1文字とか2byteが1文字とかある
            // https://seiai.ed.jp/sys/text/java/utf8table.html
            // 1byteが1文字
            if (in_char < 0x80) return 1;
            // 2byteが1文字
            else if (in_char < 0xE0)
                return 2;
            // 3byteが1文字
            else if (in_char < 0xF0)
                return 3;
            // 4byteが1文字
            else if (in_char < 0xF8)
                return 4;
            // 5byteが1文字
            else if (in_char < 0xFC)
                return 5;

            // 6byteが1文字
            return 6;
        }
#endif
    }  // namespace Local

    StringBase::StringBase(HE::Char* in_szBuff, const HE::Uint32 in_uCapacity,
                           const HE::Bool in_bUseBuff)
    {
        HE_ASSERT(in_szBuff && "文字列を格納する文字列バッファがない");
        HE_ASSERT(0 < in_uCapacity && "格納する文字最大数が0になっている");

        this->_szBuff    = in_szBuff;
        this->_uCapacity = in_uCapacity;
        if (in_bUseBuff == FALSE)
        {
            HE_CLS_MEM(this->_szBuff, sizeof(HE::Char) * in_uCapacity);
            this->_szBuff[0] = HE_STR_END_POINT;
        }
    }

    StringBase& StringBase::Replace(const HE::Char* in_szOld, const HE::Char* in_szNew)
    {
        if (in_szOld == NULL) return *this;
        if (in_szNew == NULL) return *this;

        HE::Uint32 uOldLength = static_cast<HE::Uint32>(HE_STR_LENGTH(in_szOld));
        HE::Ptr pNewLength    = static_cast<HE::Ptr>(HE_STR_LENGTH(in_szNew));
        const HE::Char* szSrc = this->_szBuff;

        // 対象を探す
        do
        {
            const HE::Char* szFind = HE_STR_STR(szSrc, in_szOld);
            if (szFind == NULL) break;

            // 見つかった場所を抜いて新しい文字列に差し替え
            const HE::Ptr pAddr            = szFind - this->_szBuff;
            const HE::Uint32 uReplaceIndex = static_cast<HE::Uint32>(pAddr);

            this->Remove(uReplaceIndex, uOldLength);
            this->Insert(uReplaceIndex, in_szNew);
            szSrc = this->_szBuff + pAddr + pNewLength;

            // もうこれ以上置き換えられない
            if (static_cast<HE::Uint32>(szSrc - this->_szBuff) >= this->_uCapacity) break;

        } while (*szSrc != HE_STR_END_POINT);

        return *this;
    }

    StringBase& StringBase::Insert(const HE::Uint32 in_uIndex, const HE::Char* in_szInsert)
    {
        HE::Char* szBuffEnd     = this->_szBuff + this->_uCapacity - 1;
        HE::Uint32 uOriginCount = this->Size();
        HE::Uint32 uInsertCount =
            ((!in_szInsert) ? 1 : static_cast<HE::Uint32>(HE_STR_LENGTH(in_szInsert)));

        // 後ろに追加して終わり
        if (in_uIndex >= uOriginCount) return (*this) += in_szInsert;

        // 挿入サイズをチェックする
        if (in_uIndex + uInsertCount >= this->_uCapacity - 1)
            uInsertCount = this->_uCapacity - in_uIndex - 1;

        // 挿入箇所以降の文字を後ろへスライド
        {
            const HE::Char* cszSrcTopAddr = this->_szBuff + in_uIndex;
            HE::Char* szSrcTail           = this->_szBuff + uOriginCount;
            HE::Char* szDst               = this->_szBuff + uOriginCount + uInsertCount;

            // 溢れチェック
            if (szDst > szBuffEnd)
            {
                HE::Ptr cutLen = szDst - szBuffEnd;

                szDst -= cutLen;
                szSrcTail -= cutLen;
                *szSrcTail = HE_STR_END_POINT;
            }
            HE_ASSERT(*szSrcTail == HE_STR_END_POINT);

            // コピーする
            for (const HE::Char* szSrc = szSrcTail; szSrc >= cszSrcTopAddr; --szSrc, --szDst)
                *szDst = *szSrc;
        }

        // 挿入処理
        {
            HE::Char* szDst       = this->_szBuff + in_uIndex;
            const HE::Char* szSrc = in_szInsert;

            if (in_szInsert)
            {
                for (HE::Uint32 i = 0; i < uInsertCount; ++i) *szDst++ = *szSrc++;
            }
            else
            {
                *szDst++ = HE_STR_END_POINT;
            }
        }

        return *this;
    }

    StringBase& StringBase::Remove(const HE::Uint32 in_uIndex, const HE::Uint32 in_uCount)
    {
        HE::Uint32 uCapacity = this->Capacity();
        HE::Char* szDst      = this->_szBuff + ((in_uIndex > uCapacity) ? uCapacity : in_uIndex);
        const HE::Char* szSrc =
            this->_szBuff +
            (((in_uIndex + in_uCount) > uCapacity) ? uCapacity : (in_uIndex + in_uCount));
        const HE::Char* szSrcEnd = this->_szBuff + uCapacity;

        while (szSrc <= szSrcEnd) *szDst++ = *szSrc++;

        return *this;
    }

    StringBase& StringBase::Format(const HE::Char* in_szFormat, ...)
    {
        va_list vlist;
        va_start(vlist, in_szFormat);
        this->FormatV(in_szFormat, vlist);
        va_end(vlist);

        return *this;
    }

    StringBase& StringBase::FormatV(const HE::Char* in_szFormat, va_list in_vlist)
    {
        if (in_szFormat && this->_uCapacity > 1)
        {
            HE_STR_VSNPRINTF(this->_szBuff, this->_uCapacity, this->_uCapacity - 1, in_szFormat,
                             in_vlist);
            this->_szBuff[this->_uCapacity - 1] = HE_STR_END_POINT;
        }
        else
        {
            this->Clear();
        }

        return *this;
    }

    HE::Sint32 StringBase::Find(const HE::Char* in_szName, const HE::Uint32 in_uStart) const
    {
        const HE::Uint32 uCount = this->Size();
        if (uCount <= in_uStart) return uCount;

        const HE::Char* szFind = HE_STR_STR(this->_szBuff + in_uStart, in_szName);
        if (szFind == NULL) return uCount;

        return static_cast<HE::Sint32>(szFind - this->_szBuff);
    }

    HE::Uint32 StringBase::Length() const
    {
#if !defined(HE_CHARACTER_CODE_UTF8) && defined(HE_WIN)
        return static_cast<HE::Uint32>(::wcslen((this->_szBuff)))
#else
        HE::Uint32 _uSize = this->Size();
        HE::Uint32 uLen   = 0;

        HE::Uint32 uOffset = 0;
        HE::Uint32 i       = 0;
        while (i < _uSize)
        {
            HE::Uint8 c = static_cast<HE::Uint8>(this->_szBuff[i]);
            // 終端があれば終了する
            if (c == 0x00) break;

            uOffset = Local::GetUTF8CharOffset(c);

            i += uOffset;
            uLen += 1;
        }

        return uLen;
#endif
    }

    /// <summary>
    /// 文字列をハッシュ化して返す
    /// </summary>
    HE::Uint64 StringBase::Hash() const
    {
        return HashName(this->Str());
    }

    /// <summary>
    /// HE::UTF8として出力
    /// 文字列をUTF-8として利用したい場合に利用
    /// </summary>
    void StringBase::OutputUTF8(HE::UTF8* out, const HE::Uint32 in_uOutBuffLen) const
    {
        HE_ASSERT(out);
        HE_CLS_MEM(out, in_uOutBuffLen);

        HE::Uint32 uOutputTextMaxLen = in_uOutBuffLen;
        if (in_uOutBuffLen <= this->Capacity())
        {
            uOutputTextMaxLen = this->Capacity();
        }

#ifdef HE_USE_WCHAR
        // wchar_t型をutf8のcharに変えて出力
        // WideからHE::UTF8にした時の文字列数を取得
        {
            HE::Sint32 uUTF8TextLen = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS,
                                                          this->Str(), -1, NULL, 0, NULL, NULL);
            if (uUTF8TextLen <= 0) return;

            uOutputTextMaxLen = HE_MIN(static_cast<HE::Uint32>(uUTF8TextLen), uOutputTextMaxLen);
        }

        // HE::UTF8の文字列数を出力
        WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, this->Str(), -1, out, uOutputTextMaxLen,
                            NULL, NULL);
#else
        ::memcpy_s(out, in_uOutBuffLen * sizeof(out[0]), this->_szBuff,
                   this->Size() * sizeof(this->_szBuff[0]));
#endif
    }

    /// <summary>
    /// TODO: WCharの文字列を出力
    /// </summary>
    void StringBase::OutputW(HE::WChar* out, const HE::Uint32 in_uLen) const
    {
        HE_ASSERT(out);
        HE_ASSERT(in_uLen <= this->_uCapacity);

        // windowsの場合
        // linuxは未対応
#ifdef HE_WIN
        // 利用する文字数を取得
        HE::Sint32 iUseSize = ::MultiByteToWideChar(CP_UTF8, 0, this->Str(), in_uLen, NULL, 0);
        // 利用する文字数が制限を超えていないかチェック
        HE_ASSERT(iUseSize <= in_uLen);

        // HE::UTF8文字列からUTF16の文字列に変える
        ::MultiByteToWideChar(CP_UTF8, 0, this->Str(), this->_uCapacity, out, iUseSize);
#endif
    }

    /// <summary>
    /// 文字列を出力
    /// </summary>
    void StringBase::Output(HE::Char* out, const HE::Uint32 in_uLen) const
    {
        HE_ASSERT_RETURN(out);
        HE_ASSERT_RETURN(0 < in_uLen);

        // 文字数の長さではなく文字データとしてサイズにしないとコピーがうまくいかない
        HE_STR_COPY_S(out, in_uLen, this->_szBuff, this->Size());
    }

    StringBase& StringBase::_Copy(const HE::Char* in_szName, const HE::Uint32 in_uLen)
    {
        HE_ASSERT(in_szName && "コピーしたい文字列がない");
        HE_ASSERT(0 < this->_uCapacity && "コピー先のバッファサイズがない");
        if (in_szName && 0 < this->_uCapacity)
        {
            auto bSuccess =
                HE_STR_SUCCESS(HE_STR_COPY_S(this->_szBuff, this->_uCapacity, in_szName, in_uLen));
            HE_ASSERT(bSuccess && "文字列コピーに失敗");
        }
        else
        {
            this->Clear();
        }

        return *this;
    }

    StringBase& StringBase::_Add(const HE::Char* in_szName)
    {
        if (in_szName && this->_uCapacity > 0)
        {
            HE::Uint32 _uCount   = this->Size();
            HE::Sint32 iCatCount = static_cast<HE::Sint32>(this->_uCapacity - _uCount - 1);
            HE_ASSERT(0 < iCatCount &&
                      "文字列の長さがバッファサイズを超えて文字列の追加ができない");

            if (iCatCount > 0)
            {
                auto bSuccess = HE_STR_SUCCESS(
                    HE_STR_COPY_S(this->_szBuff + _uCount,
                                  static_cast<HE::Sint32>(this->_uCapacity - _uCount), in_szName,
                                  iCatCount));
                HE_ASSERT(bSuccess && "文字列コピーに失敗");

                this->_szBuff[this->_uCapacity - 1] = '\0';
            }
        }
        else
        {
            this->Clear();
        }

        return *this;
    }

    StringBase& StringBase::_Add(const HE::Char c)
    {
        HE::Uint32 _uCount = this->Size();
        HE_ASSERT(_uCount + 1 < this->_uCapacity &&
                  "文字列の長さがバッファサイズを超えて文字の追加ができない");

        if (_uCount + 1 < this->_uCapacity)
        {
            this->_szBuff[_uCount]     = c;
            this->_szBuff[_uCount + 1] = '\0';
        }

        return *this;
    }

    StringBase::IteratorChar& StringBase::IteratorChar::operator++()
    {
#if !defined(HE_CHARACTER_CODE_UTF8) && defined(HE_WIN)
        ++this->_sIndex;
#else
        // UTF8の場合、日本語などワイド文字だとオフセット値が異なる
        HE::Uint32 uOffset = Local::GetUTF8CharOffset(this->_pStr[this->_sIndex]);
        this->_sIndex += uOffset;
#endif
        return *this;
    }

    HE::Char* StringBase::IteratorChar::operator*()
    {
#if !defined(HE_CHARACTER_CODE_UTF8) && defined(HE_WIN)
        this->_aChar[0] = this->_pStr[this->_sIndex];
        this->_aChar[1] = NULL;
#else
        HE::Uint32 uOffset = Local::GetUTF8CharOffset(this->_pStr[this->_sIndex]);
        for (auto i = 0; i < uOffset; ++i)
        {
            this->_aChar[i] = this->_pStr[this->_sIndex + i];
        }
        this->_aChar[uOffset] = HE_STR_CHAR_EMPTY;

#endif
        return this->_aChar;
    }

    FixedString16 g_szTempFixedString16;
    FixedString32 g_szTempFixedString32;
    FixedString64 g_szTempFixedString64;
    FixedString128 g_szTempFixedString128;
    FixedString256 g_szTempFixedString256;
    FixedString512 g_szTempFixedString512;
    FixedString1024 g_szTempFixedString1024;

    HE::UTF8 g_szTempFixedUTF8_16[16]     = "";
    HE::UTF8 g_szTempFixedUTF8_32[32]     = "";
    HE::UTF8 g_szTempFixedUTF8_64[64]     = "";
    HE::UTF8 g_szTempFixedUTF8_128[128]   = "";
    HE::UTF8 g_szTempFixedUTF8_256[256]   = "";
    HE::UTF8 g_szTempFixedUTF8_512[512]   = "";
    HE::UTF8 g_szTempFixedUTF8_1024[1024] = "";

    /// <summary>
    /// UTF8の文字からユニコード値にして返す
    /// </summary>
    HE::Uint32 GetUTF8CharToUnicode(const HE::UTF8* in_pUTF8Char)
    {
        HE::UTF8 c = in_pUTF8Char[0];
        // 1バイト文字 (ASCII)
        if ((c & 0x80) == 0x00)
        {
            return c;
        }

        // 2バイト文字
        if ((c & 0xE0) == 0xC0)
        {
            return ((c & 0x1F) << 6) | (in_pUTF8Char[1] & 0x3F);
        }

        // 3バイト文字
        if ((c & 0xF0) == 0xE0)
        {
            return ((c & 0x0F) << 12) | ((in_pUTF8Char[1] & 0x3F) << 6) | (in_pUTF8Char[2] & 0x3F);
        }

        // 4バイト文字
        if ((c & 0xF8) == 0xF0)
        {
            return ((c & 0x07) << 18) | ((in_pUTF8Char[1] & 0x3F) << 12) |
                   ((in_pUTF8Char[2] & 0x3F) << 6) | (in_pUTF8Char[3] & 0x3F);
        }

        return 0;
    }

    /// <summary>
    /// UTF8文字列を区切る
    /// </summary>
    void SplitWithUTF8(std::function<void(const HE::UTF8*, const HE::Uint32)> in_resultFunc,
                       HE::UTF8* in_szName, const HE::Uint32 in_uLen, const HE::UTF8* in_szDelim)
    {
        HE::UTF8* szToken = std::strtok(in_szName, in_szDelim);
        HE::Uint32 uCount = 1;
        while (szToken != nullptr)
        {
            in_resultFunc(szToken, uCount);
            ++uCount;

            szToken = std::strtok(nullptr, in_szDelim);
        }
    }

}  // namespace Core::Common
