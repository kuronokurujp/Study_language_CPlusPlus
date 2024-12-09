#include "ParamaterAssetData.h"

namespace Game::Asset
{
    HE::Bool ParamaterAssetData::_VLoad(Platform::FileInterface& in_rFileSystem)
    {
        if (AssetManager::AssetDataJson::_VLoad(in_rFileSystem) == FALSE) return FALSE;

        // バージョン値を取得
        this->_version = this->VGetUInt32({"version"});

        return TRUE;
    }

    void ParamaterAssetData::_VUnload()
    {
        AssetDataJson::_VUnload();
    }

    HE::Uint32 ParamaterAssetData::GetUInt32ByIdData(const HE::UTF8* in_pIdName,
                                                     const HE::UTF8* in_pFieldName)
    {
        return this->VGetUInt32({"data", in_pIdName, in_pFieldName});
    }

    HE::Sint32 ParamaterAssetData::GetSInt32ByIdData(const HE::UTF8* in_pIdName,
                                                     const HE::UTF8* in_pFieldName)
    {
        return this->VGetSInt32({"data", in_pIdName, in_pFieldName});
    }

    HE::Float32 ParamaterAssetData::GetFloat32ByIdData(const HE::UTF8* in_pIdName,
                                                       const HE::UTF8* in_pFieldName)
    {
        return this->VGetFloat32({"data", in_pIdName, in_pFieldName});
    }

    Core::Common::FixedString1024 ParamaterAssetData::GetCharByIdData(const HE::UTF8* in_pIdName,
                                                                      const HE::UTF8* in_pFieldName)
    {
        return this->VGetChar({"data", in_pIdName, in_pFieldName});
    }

    HE::Bool ParamaterAssetData::IsId(const HE::UTF8* in_pIdName)
    {
        return this->IsToken({"data", in_pIdName});
    }

}  // namespace Game::Asset
