#include "AssetDataJson.h"

// 依存モジュール
#include "Engine/Platform/PlatformFile.h"

namespace AssetManager
{
    static Bool _OutputValueBySimdJson(simdjson::fallback::ondemand::value* out,
                                       simdjson::ondemand::document& in_rDoc,
                                       const std::initializer_list<const UTF8*>& in_rTokens)
    {
        HE_ASSERT(out);
        HE_ASSERT(0 < in_rTokens.size());

        try
        {
            auto itr = in_rTokens.begin();

            auto v = in_rDoc[*itr];
            HE_ASSERT(v.error() == simdjson::error_code::SUCCESS);

            // 複数トークンでの取得
            ++itr;
            for (; itr != in_rTokens.end(); ++itr)
            {
                v = v[*itr];
                HE_ASSERT(v.error() == simdjson::error_code::SUCCESS);
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

    Uint32 AssetDataJson::VGetUInt32(const std::initializer_list<const UTF8*>& in_rTokens)
    {
        simdjson::fallback::ondemand::value val;
        if (_OutputValueBySimdJson(&val,
                                   *(reinterpret_cast<simdjson::ondemand::document*>(this->_pDoc)),
                                   in_rTokens) == FALSE)
            return 0;

        HE_ASSERT(val.is_integer());
        return static_cast<Uint32>(val.get_int64().value_unsafe());
    }

    Float32 AssetDataJson::VGetFloat32(const std::initializer_list<const UTF8*>& in_rTokens)
    {
        simdjson::fallback::ondemand::value val;
        if (_OutputValueBySimdJson(&val,
                                   *(reinterpret_cast<simdjson::ondemand::document*>(this->_pDoc)),
                                   in_rTokens) == FALSE)
            return 0;

        return static_cast<Float32>(val.get_double().value_unsafe());
    }

    Core::Common::FixString1024 AssetDataJson::VGetChar(
        const std::initializer_list<const UTF8*>& in_rTokens)
    {
        Core::Common::FixString1024 str;

        simdjson::fallback::ondemand::value val;
        if (_OutputValueBySimdJson(&val,
                                   *(reinterpret_cast<simdjson::ondemand::document*>(this->_pDoc)),
                                   in_rTokens) == FALSE)
            return str;

        HE_ASSERT(val.is_string());
        // unsafeの方が高速なのだが, 文字列の中にゴミの値が入っていた
        str = val.get_string().value();

        return str;
    }

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
                // TODO: メモリページを外部から指定できるようにする
                pReadTmpBuff = HE_NEW_MEM_ARRAY(UTF8, iMemSize, 0);
                ::memset(pReadTmpBuff, '\0', iMemSize);

                // ファイルの読み込み
                if (in_rFileSystem.VFileRead(pReadTmpBuff, this->_fileHandle, iSize))
                {
                    // 読み込んだメモリをjsonデータとして展開
                    // 展開時にjsonを展開するためのメモリ確保をする
                    pReadTmpBuff[iSize] = '\n';
                    HE_ASSERT(simdjson::validate_utf8(pReadTmpBuff, iMemSize));

                    this->_json =
                        HE_MAKE_CUSTOM_UNIQUE_PTR((simdjson::padded_string), pReadTmpBuff, iSize);
                    this->_parser =
                        HE_MAKE_CUSTOM_UNIQUE_PTR((simdjson::ondemand::parser), (iSize * 2));
                    {
                        this->_pDoc = HE_NEW_MEM(simdjson::ondemand::document, 0);
                        simdjson::ondemand::document* pDoc =
                            reinterpret_cast<simdjson::ondemand::document*>(this->_pDoc);

                        auto resultCode = this->_parser->iterate(*this->_json).get(*pDoc);
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

        return bRet;
    }

    void AssetDataJson::_VUnload()
    {
        HE_SAFE_DELETE_UNIQUE_PTR(this->_json);
        HE_SAFE_DELETE_UNIQUE_PTR(this->_parser);

        HE_SAFE_DELETE_MEM(this->_pDoc);
    }

}  // namespace AssetManager
