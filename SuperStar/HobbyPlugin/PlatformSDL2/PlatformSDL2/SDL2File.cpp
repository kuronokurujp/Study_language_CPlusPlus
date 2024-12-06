#include "SDL2File.h"

namespace PlatformSDL2
{
    const Core::Common::Handle File::VFileOpen(const Core::File::Path& in_rPath)
    {
        /*
        auto handle = FileRead_open(in_rPath.Str(), FALSE);
        if (handle == 0) return Core::Common::Handle();

        // ファイルハンドルをハンドルインデックスにしている
        Core::Common::Handle fileHandle;
        fileHandle.SetIndex(handle);
        this->_mFileHandle.Add(fileHandle.Magic(), fileHandle);

        return fileHandle;
        */
        return NullHandle;
    }

    Bool File::VFileRead(void* out_pBuff, const Core::Common::Handle& in_rHandle,
                         const HE::HE::Uint32 in_size)
    {
        HE_ASSERT_RETURN_VALUE(FALSE, in_rHandle.Null() == FALSE && "ファイルのハンドルがない");
        HE_ASSERT_RETURN_VALUE(FALSE, out_pBuff != NULL &&
                                          "ファイルの読み込みのバッファの先頭アドレスがない");
        HE_ASSERT_RETURN_VALUE(FALSE, 0 < in_size && "ファイルの読み込みサイズが0以下");

        /*
                const HE::HE::Uint32 resultCode = FileRead_read(out_pBuff, in_size, in_rHandle.Index());
                if (resultCode == -1)
                {
                    HE_ASSERT(0 && "ファイルの読み込み失敗");
                    return FALSE;
                }
                */

        return TRUE;
    }

    HE::HE::Uint32 File::VFileSize(const Core::Common::Handle& in_rHandle)
    {
        HE_ASSERT_RETURN_VALUE(0, in_rHandle.Null() == FALSE);
        // ファイル読み込みのサイズがHE::HE::Uint32なので合わせる
        // return static_cast<HE::HE::Uint32>(FileRead_size_handle(in_rHandle.Index()));
        return 0;
    }

    Bool File::VFileClose(const Core::Common::Handle& in_rHandle)
    {
        HE_ASSERT_RETURN_VALUE(FALSE, in_rHandle.Null() == FALSE);

        /*
                this->_mFileHandle.Erase(in_rHandle.Magic());
                if (FileRead_close(in_rHandle.Index()) != 0) return FALSE;
                */

        return TRUE;
    }
}  // namespace Platform::SDL2
