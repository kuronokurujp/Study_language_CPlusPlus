#include "ParamaterAssetData.h"

namespace Game::Asset
{
    HE::Bool ParamaterAssetData::_VLoad(Platform::FileInterface& in_rFileSystem)
    {
        if (AssetManager::AssetDataJson::_VLoad(in_rFileSystem) == FALSE) return FALSE;

        // バージョン値を取得
        auto spNode    = this->VGetNodeByName({HE_STR_U8_TEXT("version")});
        this->_version = spNode->VGetUInt32();

        return TRUE;
    }

    void ParamaterAssetData::_VUnload()
    {
        AssetDataJson::_VUnload();
    }

    HE::Uint32 ParamaterAssetData::GetUInt32ByName(const HE::UTF8* in_pIdName,
                                                   const HE::UTF8* in_pFieldName)
    {
        auto spNode = this->VGetNodeByName({HE_STR_U8_TEXT("data"), in_pIdName, in_pFieldName});
        return spNode->VGetUInt32();
    }

    HE::Sint32 ParamaterAssetData::GetSInt32ByName(const HE::UTF8* in_pIdName,
                                                   const HE::UTF8* in_pFieldName)
    {
        auto spNode = this->VGetNodeByName({HE_STR_U8_TEXT("data"), in_pIdName, in_pFieldName});
        return spNode->VGetSInt32();
    }

    HE::Float32 ParamaterAssetData::GetFloat32ByName(const HE::UTF8* in_pIdName,
                                                     const HE::UTF8* in_pFieldName)
    {
        auto spNode = this->VGetNodeByName({HE_STR_U8_TEXT("data"), in_pIdName, in_pFieldName});
        return spNode->VGetFloat32();
    }

    void ParamaterAssetData::OutputStringByName(Core::Common::StringBase* out,
                                                const HE::UTF8* in_pIdName,
                                                const HE::UTF8* in_pFieldName)
    {
        auto spNode = this->VGetNodeByName({HE_STR_U8_TEXT("data"), in_pIdName, in_pFieldName});
        spNode->VOutputString(out);
    }

    HE::Uint32 ParamaterAssetData::GetUInt32ByIndex(const HE::Uint32 in_uIndex,
                                                    const HE::UTF8* in_pFieldName)
    {
        auto spNode =
            this->VGetNodeByLevel({HE_STR_U8_TEXT("data")}, static_cast<HE::Sint32>(in_uIndex));
        if (spNode->IsNone()) return 0;

        return spNode->VGetUInt32(in_pFieldName);
    }

    HE::Sint32 ParamaterAssetData::GetSInt32ByIndex(const HE::Uint32 in_uIndex,
                                                    const HE::UTF8* in_pFieldName)
    {
        auto spNode =
            this->VGetNodeByLevel({HE_STR_U8_TEXT("data")}, static_cast<HE::Sint32>(in_uIndex));
        if (spNode->IsNone()) return 0;

        return spNode->VGetSInt32(in_pFieldName);
    }

    HE::Float32 ParamaterAssetData::GetFloat32ByIndex(const HE::Uint32 in_uIndex,
                                                      const HE::UTF8* in_pFieldName)
    {
        auto spNode =
            this->VGetNodeByLevel({HE_STR_U8_TEXT("data")}, static_cast<HE::Sint32>(in_uIndex));
        if (spNode->IsNone()) return 0;

        return spNode->VGetFloat32(in_pFieldName);
    }

    void ParamaterAssetData::OutputStringByIndex(Core::Common::StringBase* out,
                                                 const HE::Uint32 in_uIndex,
                                                 const HE::UTF8* in_pFieldName)
    {
        auto spNode =
            this->VGetNodeByLevel({HE_STR_U8_TEXT("data")}, static_cast<HE::Sint32>(in_uIndex));
        if (spNode->IsNone()) return;

        spNode->VOutputString(out, in_pFieldName);
    }

    void ParamaterAssetData::OutputIdByIndex(Core::Common::StringBase* out,
                                             const HE::Uint32 in_uIndex)
    {
        auto spNode =
            this->VGetNodeByLevel({HE_STR_U8_TEXT("data")}, static_cast<HE::Sint32>(in_uIndex));
        if (spNode->IsNone()) return;

        *out = spNode->Name();
    }

    HE::Bool ParamaterAssetData::IsId(const HE::UTF8* in_pIdName)
    {
        return this->IsToken({"data", in_pIdName});
    }

    HE::Bool ParamaterAssetData::IsIndex(const HE::Uint32 in_uIndex)
    {
        auto spNode =
            this->VGetNodeByLevel({HE_STR_U8_TEXT("data")}, static_cast<HE::Sint32>(in_uIndex));
        return (spNode->IsNone() == FALSE);
    }

}  // namespace Game::Asset
