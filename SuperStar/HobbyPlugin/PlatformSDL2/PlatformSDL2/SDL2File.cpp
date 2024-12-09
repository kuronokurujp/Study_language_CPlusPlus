#include "SDL2File.h"

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

    const Core::Common::Handle File::VFileOpen(const Core::File::Path& in_rPath)
    {
        auto pFileRW = ::SDL_RWFromFile(in_rPath.Str(), "rb");
        HE_ASSERT_RETURN_VALUE(NullHandle, pFileRW);

        FileInfo* pFileInfo    = HE_NEW_MEM(FileInfo, 0);
        pFileInfo->_pSDLRWOpen = pFileRW;
        pFileInfo->_iFileSize  = pFileRW->size(pFileRW);

        // ファイルパスをハッシュ値にしてそれをハンドルにしている
        Core::Common::Handle fileHandle(Core::Common::HashName(in_rPath.Str()));

        this->_mFileHandle.Add(fileHandle, pFileInfo);

        return fileHandle;
    }

    HE::Bool File::VFileRead(void* out_pBuff, const Core::Common::Handle& in_rHandle,
                             const HE::Uint32 in_uSize)
    {
        HE_ASSERT_RETURN_VALUE(FALSE, in_rHandle.Null() == FALSE && "ファイルのハンドルがない");
        HE_ASSERT_RETURN_VALUE(FALSE, out_pBuff != NULL &&
                                          "ファイルの読み込みのバッファの先頭アドレスがない");
        HE_ASSERT_RETURN_VALUE(FALSE, 0 < in_uSize && "ファイルの読み込みサイズが0以下");

        auto itr  = this->_mFileHandle.FindKey(in_rHandle);
        auto size = itr->data->_pSDLRWOpen->read(itr->data->_pSDLRWOpen, out_pBuff, 1,
                                                 static_cast<size_t>(in_uSize));
        HE_ASSERT_RETURN_VALUE(FALSE, size == in_uSize);

        return TRUE;
    }

    HE::Sint32 File::VFileSize(const Core::Common::Handle& in_rHandle)
    {
        HE_ASSERT_RETURN_VALUE(0, in_rHandle.Null() == FALSE);

        auto itr = this->_mFileHandle.FindKey(in_rHandle);

        return itr->data->_iFileSize;
    }

    HE::Bool File::VFileClose(const Core::Common::Handle& in_rHandle)
    {
        HE_ASSERT_RETURN_VALUE(FALSE, in_rHandle.Null() == FALSE);

        auto itr = this->_mFileHandle.FindKey(in_rHandle);

        itr->data->_pSDLRWOpen->close(itr->data->_pSDLRWOpen);

        HE_SAFE_DELETE_MEM(itr->data);

        return TRUE;
    }
}  // namespace PlatformSDL2
