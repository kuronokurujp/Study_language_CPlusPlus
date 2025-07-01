#include "PlatformSDL2/SDL2File.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "Engine/Common/Hash.h"

// 利用パッケージ
#include "SDL2/SDL.h"

namespace PlatformSDL2
{
    struct FileInfo
    {
        ::SDL_RWops* _pSDLRWOpen = NULL;
        HE::Sint32 _iFileSize    = 0;
    };

    void File::SetCurrentDir(const Core::File::Path& in_rPath)
    {
        this->_currentDirPath = in_rPath;
    }

    const Core::Common::Handle File::VFileOpen(const Core::File::Path& in_rPath)
    {
        auto path = Core::File::Path(this->_currentDirPath.Str(), in_rPath.Str());

        auto pFileRW = ::SDL_RWFromFile(path.Str(), "rb");
        HE_ASSERT_RETURN_VALUE(NullHandle, pFileRW);

        FileInfo* pFileInfo    = HE_NEW_MEM(FileInfo, 0);
        pFileInfo->_pSDLRWOpen = pFileRW;
        pFileInfo->_iFileSize  = pFileRW->size(pFileRW);

        // ファイルパスをハッシュ値にしてそれをハンドルにしている
        Core::Common::Handle fileHandle(Core::Common::HashName(path.Str()));

        this->_mFileHandle.Add(fileHandle, pFileInfo);

        return fileHandle;
    }

    HE::Uint32 File::VFileRead(void* out_pBuff, const Core::Common::Handle& in_rHandle,
                               const HE::Uint32 in_uSize)
    {
        HE_ASSERT_RETURN_VALUE(FALSE, in_rHandle.Null() == FALSE && "ファイルのハンドルがない");
        HE_ASSERT_RETURN_VALUE(FALSE, out_pBuff != NULL &&
                                          "ファイルの読み込みのバッファの先頭アドレスがない");
        HE_ASSERT_RETURN_VALUE(FALSE, 0 < in_uSize && "ファイルの読み込みサイズが0以下");

        auto itr         = this->_mFileHandle.FindKey(in_rHandle);
        HE::Uint32 uSize = 0;
        uSize            = ::SDL_RWread(itr->_data->_pSDLRWOpen, out_pBuff, 1, in_uSize);

        return uSize;
    }

    HE::Sint32 File::VFileSize(const Core::Common::Handle& in_rHandle)
    {
        HE_ASSERT_RETURN_VALUE(0, in_rHandle.Null() == FALSE);

        auto itr = this->_mFileHandle.FindKey(in_rHandle);

        return itr->_data->_iFileSize;
    }

    HE::Bool File::VFileClose(const Core::Common::Handle& in_rHandle)
    {
        HE_ASSERT_RETURN_VALUE(FALSE, in_rHandle.Null() == FALSE);

        auto itr = this->_mFileHandle.FindKey(in_rHandle);

        itr->_data->_pSDLRWOpen->close(itr->_data->_pSDLRWOpen);

        HE_SAFE_DELETE_MEM(itr->_data);

        return TRUE;
    }

    File::BinaryData File::VLoadBinary(const Core::File::Path& in_rPath)
    {
        // ファイルをバイナル形式でロード
        auto handle = this->VFileOpen(in_rPath.Str());
        HE_ASSERT_RETURN_VALUE((BinaryData(NULL, 0)), handle.Null() == FALSE);

        auto uSize = this->VFileSize(handle);

        // TODO: メモリページは指定したい
        void* pData = HE_ALLOC_MEM(uSize, 0);
        if (pData != NULL)
        {
            ::memset(pData, 0, uSize);

            // ロード
            HE::Bool bRet = FALSE;

            if (this->VFileRead(pData, handle, uSize))
            {
                bRet = TRUE;
            }
            else
            {
                HE_SAFE_DELETE_MEM(pData);
            }
            HE_ASSERT(bRet && "ファイル読み込みに失敗");
        }

        this->VFileClose(handle);

        return BinaryData(pData, uSize);
    }

    std::tuple<HE::Char*, HE::Uint32> File::VLoadText(const Core::File::Path& in_rPath)
    {
        auto path = Core::File::Path(this->_currentDirPath.Str(), in_rPath.Str());
        // ファイルをバイナル形式でロード
        auto handle = this->VFileOpen(in_rPath.Str());
        HE_ASSERT_RETURN_VALUE((std::tuple<HE::Char*, HE::Uint32>(NULL, 0)),
                               handle.Null() == FALSE);

        auto uSize = this->VFileSize(handle);

        // TODO: メモリページは指定したい
        HE::Char* pData = reinterpret_cast<HE::Char*>(HE_ALLOC_MEM(uSize + 1, 0));
        if (pData != NULL)
        {
            ::memset(pData, 0, uSize + 1);

            // ロード
            HE::Bool bRet = FALSE;

            if (this->VFileRead(pData, handle, uSize))
            {
                bRet         = TRUE;
                pData[uSize] = HE_STR_TEXT('\0');
            }
            else
            {
                HE_SAFE_DELETE_MEM(pData);
            }
            HE_ASSERT(bRet && "ファイル読み込みに失敗");
        }

        this->VFileClose(handle);

        return std::tuple<HE::Char*, HE::Uint32>(pData, uSize);
    }
}  // namespace PlatformSDL2
