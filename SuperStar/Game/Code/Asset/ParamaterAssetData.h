#pragma once

#include "Engine/MiniEngine.h"

// アセットファイルをロードするので必要
#include "AssetManagerModule.h"

namespace Game::Asset
{
    /// <summary>
    /// ゲームのパラメータアセット
    /// </summary>
    class ParamaterAssetData final : public AssetManager::AssetDataJson
    {
    public:
        /// <summary>
        /// パラメータのバージョン値
        /// </summary>
        inline HE::Uint32 Version() const { return this->_version; }

        // パラメータの取得
        HE::Uint32 GetUInt32ByIdData(const HE::UTF8* in_pIdName, const HE::UTF8* in_pFieldName);
        HE::Sint32 GetSInt32ByIdData(const HE::UTF8* in_pIdName, const HE::UTF8* in_pFieldName);
        HE::Float32 GetFloat32ByIdData(const HE::UTF8* in_pIdName, const HE::UTF8* in_pFieldName);
        Core::Common::FixedString1024 GetCharByIdData(const HE::UTF8* in_pIdName,
                                                      const HE::UTF8* in_pFieldName);

        HE::Bool IsId(const HE::UTF8* in_pIdName);

    protected:
        // パラメータ取得用のを別途用意していているので汎用取得のは使えないようにする
        virtual HE::Uint32 VGetUInt32(
            const std::initializer_list<const HE::UTF8*>& in_rTokens) override final
        {
            return AssetDataJson::VGetUInt32(in_rTokens);
        }

        virtual HE::Sint32 VGetSInt32(
            const std::initializer_list<const HE::UTF8*>& in_rTokens) override final
        {
            return AssetDataJson::VGetSInt32(in_rTokens);
        }

        virtual HE::Float32 VGetFloat32(
            const std::initializer_list<const HE::UTF8*>& in_rTokens) override final
        {
            return AssetDataJson::VGetFloat32(in_rTokens);
        }

        virtual Core::Common::FixedString1024 VGetChar(
            const std::initializer_list<const HE::UTF8*>& in_rTokens) override final
        {
            return AssetDataJson::VGetChar(in_rTokens);
        }

    protected:
        virtual HE::Bool _VLoad(::Platform::FileInterface&) override final;
        virtual void _VUnload() override final;

    private:
        HE::Uint32 _version = 0;
    };
}  // namespace Game::Asset
