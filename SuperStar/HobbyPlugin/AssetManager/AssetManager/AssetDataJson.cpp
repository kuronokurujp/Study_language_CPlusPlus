#include "AssetDataJson.h"

// 依存モジュール
#include "Engine/Platform/PlatformFile.h"

namespace AssetManager
{
    namespace Local
    {
        static HE::Bool OutputValueBySimdJson(
            simdjson::fallback::ondemand::value* out, simdjson::ondemand::document& in_rDoc,
            const std::initializer_list<const HE::UTF8*>& in_rTokens)
        {
            HE_ASSERT(out);
            HE_ASSERT(0 < in_rTokens.size());

            try
            {
                auto itr = in_rTokens.begin();

                auto v     = in_rDoc[*itr];
                auto error = v.error();
                if (error != simdjson::error_code::SUCCESS)
                {
                    HE_PG_LOG_LINE(HE_STR_TEXT("error(%d): Token(%s)"), error, *itr);
                    return FALSE;
                }

                // 複数トークンでの取得
                ++itr;
                for (; itr != in_rTokens.end(); ++itr)
                {
                    v     = v[*itr];
                    error = v.error();
                    if (error != simdjson::error_code::SUCCESS)
                    {
                        HE_PG_LOG_LINE(HE_STR_TEXT("error(%d): Token(%s)"), error, *itr);
                        return FALSE;
                    }
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
    }  // namespace Local

    HE::Uint32 AssetDataJson::VGetUInt32(const std::initializer_list<const HE::UTF8*>& in_rTokens)
    {
        simdjson::fallback::ondemand::value val;
        if (Local::OutputValueBySimdJson(&val,
                                         *(reinterpret_cast<simdjson::ondemand::document*>(
                                             this->_pDoc)),
                                         in_rTokens) == FALSE)
            return 0;

        HE_ASSERT(val.is_integer());
        return static_cast<HE::Uint32>(val.get_int64().value_unsafe());
    }

    HE::Sint32 AssetDataJson::VGetSInt32(const std::initializer_list<const HE::UTF8*>& in_rTokens)
    {
        return static_cast<HE::Sint32>(this->VGetUInt32(in_rTokens));
    }

    HE::Float32 AssetDataJson::VGetFloat32(const std::initializer_list<const HE::UTF8*>& in_rTokens)
    {
        simdjson::fallback::ondemand::value val;
        if (Local::OutputValueBySimdJson(&val,
                                         *(reinterpret_cast<simdjson::ondemand::document*>(
                                             this->_pDoc)),
                                         in_rTokens) == FALSE)
            return 0;

        return static_cast<HE::Float32>(val.get_double().value_unsafe());
    }

    Core::Common::FixedString1024 AssetDataJson::VGetChar(
        const std::initializer_list<const HE::UTF8*>& in_rTokens)
    {
        Core::Common::FixedString1024 str;

        simdjson::fallback::ondemand::value val;
        if (Local::OutputValueBySimdJson(&val,
                                         *(reinterpret_cast<simdjson::ondemand::document*>(
                                             this->_pDoc)),
                                         in_rTokens) == FALSE)
            return str;

        HE_ASSERT(val.is_string());
        // unsafeの方が高速なのだが, 文字列の中にゴミの値が入っていた
        str = val.get_string().value();

        return str;
    }

    HE::Bool AssetDataJson::IsToken(const std::initializer_list<const HE::UTF8*>& in_rTokens)
    {
        simdjson::fallback::ondemand::value val;
        if (Local::OutputValueBySimdJson(&val,
                                         *(reinterpret_cast<simdjson::ondemand::document*>(
                                             this->_pDoc)),
                                         in_rTokens) == FALSE)
            return FALSE;

        return TRUE;
    }

    HE::Bool AssetDataJson::_VLoad(Platform::FileInterface& in_rFileSystem)
    {
        HE::Bool bRet = TRUE;

        auto [pText, uSize] = in_rFileSystem.VLoadText(this->_path);
        try
        {
            // 読み込んだメモリをjsonデータとして展開
            // 展開時にjsonを展開するためのメモリ確保をする
            HE_ASSERT(simdjson::validate_utf8(pText, uSize));

            this->_json   = HE_MAKE_CUSTOM_UNIQUE_PTR((simdjson::padded_string), pText, uSize);
            this->_parser = HE_MAKE_CUSTOM_UNIQUE_PTR((simdjson::ondemand::parser), (uSize * 2));
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
                    HE_LOG_LINE(HE_STR_TEXT("%s"), pText);

                    bRet = FALSE;
                }
            }
        }
        catch (const simdjson::simdjson_error& e)
        {
            HE_PG_LOG_LINE(HE_STR_TEXT("%s ファイルの扱いに失敗: %s"), this->_path.Str(), e.what());
            bRet = FALSE;
        }
        HE_SAFE_DELETE_MEM(pText);

        return bRet;
    }

    void AssetDataJson::_VUnload()
    {
        HE_SAFE_DELETE_UNIQUE_PTR(this->_json);
        HE_SAFE_DELETE_UNIQUE_PTR(this->_parser);

        HE_SAFE_DELETE_MEM(this->_pDoc);
    }

}  // namespace AssetManager
