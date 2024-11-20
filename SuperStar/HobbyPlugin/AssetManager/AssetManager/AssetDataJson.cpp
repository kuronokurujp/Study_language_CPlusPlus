#include "AssetDataJson.h"

// 依存モジュール
#include "Engine/Platform/PlatformFile.h"

namespace AssetManager
{
    Bool AssetDataJson::_VLoad(Platform::FileInterface& in_rFileSystem)
    {
        Bool bRet = TRUE;

        // ファイルを開く
        this->_fileHandle = in_rFileSystem.VFileOpen(this->_path);
        HE_ASSERT(this->_fileHandle.Null() == FALSE);
        {
            UTF8* pReadTmpBuff = NULL;
            try
            {
                // 開いたファイルのデータサイズを取得して読み込むメモリを確保
                Sint32 iSize    = in_rFileSystem.VFileSize(this->_fileHandle);
                Sint32 iMemSize = iSize + 1;
                pReadTmpBuff    = HE_NEW_MEM_ARRAY(UTF8, iMemSize, 0);
                ::memset(pReadTmpBuff, '\0', iMemSize);

                // ファイルの読み込み
                if (in_rFileSystem.VFileRead(pReadTmpBuff, this->_fileHandle, iSize))
                {
                    // 読み込んだメモリをjsonデータとして展開
                    // 展開時にjsonを展開するためのメモリ確保をする
                    pReadTmpBuff[iSize] = '\n';
                    simdjson::validate_utf8(pReadTmpBuff, iMemSize);
                    this->_json =
                        HE_MAKE_CUSTOM_UNIQUE_PTR((simdjson::padded_string), pReadTmpBuff, iSize);
                    this->_parser =
                        HE_MAKE_CUSTOM_UNIQUE_PTR((simdjson::ondemand::parser), (iSize * 2));
                    {
                        auto resultCode = this->_parser->iterate(*this->_json).get(this->_doc);
                        if (resultCode != simdjson::error_code::SUCCESS)
                        {
                            HE_PG_LOG_LINE(HE_STR_TEXT("%s ファイルエラー: %d"), this->_path.Str(),
                                           resultCode);
                            HE_LOG_LINE(HE_STR_TEXT("エラーのjson内容"));
                            HE_LOG_LINE(HE_STR_TEXT("%s"), pReadTmpBuff);

                            bRet = FALSE;
                        }
                    }
                }
                else
                {
                    HE_ASSERT(0);
                }
            }
            catch (const simdjson::simdjson_error& e)
            {
                HE_PG_LOG_LINE(HE_STR_TEXT("%s ファイルの扱いに失敗: %s"), this->_path.Str(),
                               e.what());
                bRet = FALSE;
            }
            // jsonに展開した時のメモリを利用するので読み込んだメモリを解放
            HE_SAFE_DELETE_MEM_ARRAY(pReadTmpBuff);
        }
        // ファイルを閉じる
        in_rFileSystem.VFileClose(this->_fileHandle);

        if (bRet == FALSE) return FALSE;

        return TRUE;
    }

    void AssetDataJson::_VUnload()
    {
        // json展開したメモリを解放
        this->_json.release();
        this->_parser.release();
    }

    Bool AssetDataJson::_OutputValue(simdjson::fallback::ondemand::value* out,
                                     const Char* in_szaName[], const Uint32 in_uCount)
    {
        HE_ASSERT(out);
        HE_ASSERT(0 < in_uCount);

        try
        {
            Core::Common::FixString256 str(in_szaName[0]);
            UTF8 szKey[256] = {0};
            str.OutputUTF8(szKey, HE_ARRAY_SIZE(szKey));

            auto v = this->_doc[szKey];
            HE_ASSERT(v.error() == simdjson::error_code::SUCCESS);

            // TODO: 未対応
            // 複数の取得は未対応
            for (Uint32 i = 1; i < in_uCount; ++i)
            {
                HE_ASSERT(FALSE);
                /*
                Core::Common::FixString256 str(in_szaName[i]);
                str.OutputUTF8(szKey, HE_ARRAY_SIZE(szKey));
                v = v[szKey];
                HE_ASSERT(v.error() == simdjson::error_code::SUCCESS);
                */
            }
            // 要素を出力
            v.get(*out);

            return TRUE;
        }
        catch (const simdjson::simdjson_error& e)
        {
            HE_PG_LOG_LINE(HE_STR_TEXT("json要素がない: %s"), e.what());
        }

        return FALSE;
    }

}  // namespace AssetManager
