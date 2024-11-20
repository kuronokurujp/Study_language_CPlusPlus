#include "ParamaterAssetData.h"

namespace Game::Asset
{
    Bool ParamaterAssetData::_VLoad(Platform::FileInterface& in_rFileSystem)
    {
        if (AssetManager::AssetDataJson::_VLoad(in_rFileSystem) == FALSE) return FALSE;

        // バージョン値を取得
        this->_version = this->VGetUInt32({"version"});

        auto pDoc = (reinterpret_cast<simdjson::ondemand::document*>(this->_pDoc));

        // TODO: メモリページ指定は外部から
        simdjson::simdjson_result<simdjson::fallback::ondemand::value>* pResult =
            HE_NEW_MEM(simdjson::simdjson_result<simdjson::fallback::ondemand::value>, 0);
        (*pResult)   = (*pDoc)["data"];
        this->_pData = reinterpret_cast<void*>(pResult);

        return TRUE;
    }

    void ParamaterAssetData::_VUnload()
    {
        HE_SAFE_DELETE_MEM(this->_pData);

        AssetDataJson::_VUnload();
    }

    // 指定するノードのトークン名はアルファベットと数値のみなのでUTF8型にした
    Uint32 ParamaterAssetData::GetUInt32ByIdData(const UTF8* in_pIdName, const UTF8* in_pFieldName)
    {
        return 0;
    }

    Float32 ParamaterAssetData::GetFloat32ByIdData(const UTF8* in_pIdName,
                                                   const UTF8* in_pFieldName)
    {
        return 0;
    }

    /*
        const Core::Common::StringBase&& ParamaterAssetData::GetCharByIdData(const UTF8* in_pIdName,
                                                                             const UTF8*
       in_pFieldName)
        {
            return "";
        }
        */

}  // namespace Game::Asset
