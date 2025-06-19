#include "Engine/File/Path.h"

#ifdef HE_WIN

#include <pathcch.h>

#endif

namespace Core::File
{
    void Path::operator=(const Path& in_rPath)
    {
        this->_szPath = in_rPath._szPath;
    }

    void Path::operator=(const HE::Char* in_szPath)
    {
        this->_szPath = in_szPath;
    }

    Path& Path::operator+=(const Path& in_rPath)
    {
        this->_Append(in_rPath._szPath);

        return *this;
    }

    void Path::_Append(const Core::Common::StringBase& in_szPath)
    {
        if (in_szPath.Empty()) return;

        if (this->_szPath.Empty())
        {
            this->_Set(in_szPath.Str());
        }
        else
        {
#if !defined(HE_CHARACTER_CODE_UTF8) && defined(HE_WIN)
            HE::Char szTmp[512] = {};

            // WindowsAPIを利用
            // https://learn.microsoft.com/ja-jp/windows/win32/api/pathcch/nf-pathcch-pathcchcombine
            ::PathCchCombine(szTmp, 512, this->_szPath.Str(), in_szPath);
            this->_szPath = szTmp;
#else
            // パス連結未対応
            constexpr HE::Char cDeilmiter = HE_STR_TEXT('/');

            // パスの後ろに区切り文字がある
            if (this->_szPath.LastChar() == cDeilmiter)
            {
                // 後ろに付けるパスの先頭に区切り文字がある
                if (in_szPath.FirstChar() == cDeilmiter)
                {
                    // 区切り文字が二つになるので一つ消して連結
                    auto szChar = in_szPath.Str();
                    this->_szPath += &szChar[1];

                    return;
                }

                // 後ろに付けるパスに区切り文字がないのでそのまま連結
                this->_szPath += in_szPath;
                return;
            }
            // 前のパスの後ろに区切り文字がない
            else
            {
                if (in_szPath.LastChar() == cDeilmiter)
                {
                    this->_szPath += in_szPath;
                    return;
                }
            }

            this->_szPath += cDeilmiter;
            this->_szPath += in_szPath;

#endif
        }
    }
}  // namespace Core::File
