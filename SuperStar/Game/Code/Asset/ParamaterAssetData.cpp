#include "ParamaterAssetData.h"

namespace Game::Asset
{
    HE::Bool ParamaterAssetData::_VLoad(Platform::FileInterface& in_rFileSystem)
    {
        if (AssetManager::AssetDataJson::_VLoad(in_rFileSystem) == FALSE) return FALSE;

        // バージョン値を取得
        auto spNode    = this->VGetNodeByName({"version"});
        this->_version = spNode->VGetUInt32();

        return TRUE;
    }

    void ParamaterAssetData::_VUnload()
    {
        AssetDataJson::_VUnload();
    }

    HE::Uint32 ParamaterAssetData::GetUInt32ByIdData(const HE::UTF8* in_pIdName,
                                                     const HE::UTF8* in_pFieldName)
    {
        auto spNode = this->VGetNodeByName({"data", in_pIdName, in_pFieldName});
        return spNode->VGetUInt32();
    }

    HE::Sint32 ParamaterAssetData::GetSInt32ByIdData(const HE::UTF8* in_pIdName,
                                                     const HE::UTF8* in_pFieldName)
    {
        auto spNode = this->VGetNodeByName({"data", in_pIdName, in_pFieldName});
        return spNode->VGetSInt32();
    }

    HE::Float32 ParamaterAssetData::GetFloat32ByIdData(const HE::UTF8* in_pIdName,
                                                       const HE::UTF8* in_pFieldName)
    {
        auto spNode = this->VGetNodeByName({"data", in_pIdName, in_pFieldName});
        return spNode->VGetFloat32();
    }

    void ParamaterAssetData::OutputStringByIdData(Core::Common::StringBase* out,
                                                  const HE::UTF8* in_pIdName,
                                                  const HE::UTF8* in_pFieldName)
    {
        auto spNode = this->VGetNodeByName({"data", in_pIdName, in_pFieldName});
        spNode->VOutputString(out);
    }

    HE::Bool ParamaterAssetData::IsId(const HE::UTF8* in_pIdName)
    {
        return this->IsToken({"data", in_pIdName});
    }

}  // namespace Game::Asset
