#include "AssetDataXml.h"

// 依存モジュール
#include "Engine/Platform/PlatformFile.h"

namespace AssetManager
{
    Bool AssetDataXml::_VLoad(Platform::FileInterface& in_rFileSystem)
    {
        Bool bRet = TRUE;

        // ファイルを開く
        this->_fileHandle = in_rFileSystem.VFileOpen(this->_path);
        HE_ASSERT(this->_fileHandle.Null() == FALSE);
        {
            UTF8* pReadTmpBuff = NULL;
            {
                // 開いたファイルのデータサイズを取得して読み込むメモリを確保
                Sint32 iSize    = in_rFileSystem.VFileSize(this->_fileHandle);
                Sint32 iMemSize = iSize + 1;
                pReadTmpBuff    = HE_NEW_MEM_ARRAY(UTF8, iMemSize, 0);  // new UTF8[iMemSize];
                ::memset(pReadTmpBuff, '\0', iMemSize);

                // ファイルの読み込み
                if (in_rFileSystem.VFileRead(pReadTmpBuff, this->_fileHandle, iSize))
                {
                    pugi::xml_parse_result result = this->_doc.load_string(pReadTmpBuff);
                    if (result)
                    {
                        HE_LOG_LINE(HE_STR_TEXT("XML Load Success: %s"), this->_path.Str());
                    }
                    else
                    {
                        HE_LOG_LINE(HE_STR_TEXT("Error description: %s"), result.description());
                        HE_LOG_LINE(HE_STR_TEXT("Error offset: %td (error at [...%s] "),
                                    result.offset, (pReadTmpBuff + result.offset));
                    }
                }
                else
                {
                    HE_ASSERT(0);
                }
            }
            // 展開した時のメモリを利用するので読み込んだメモリを解放
            HE_SAFE_DELETE_MEM_ARRAY(pReadTmpBuff);
        }
        // ファイルを閉じる
        in_rFileSystem.VFileClose(this->_fileHandle);

        if (bRet == FALSE) return FALSE;

        return TRUE;
    }

    void AssetDataXml::_VUnload()
    {
        this->_doc.reset();
    }

}  // namespace AssetManager
