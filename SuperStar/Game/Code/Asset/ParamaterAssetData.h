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
        inline Uint32 Version() const { return this->_version; }

        // パラメータの取得
        Uint32 GetUInt32ByIdData(const UTF8* in_pIdName, const UTF8* in_pFieldName);
        Float32 GetFloat32ByIdData(const UTF8* in_pIdName, const UTF8* in_pFieldName);
        Core::Common::FixString1024 GetCharByIdData(const UTF8* in_pIdName,
                                                    const UTF8* in_pFieldName);

    protected:
        // パラメータ取得用のを別途用意していているので汎用取得のは使えないようにする
        virtual Uint32 VGetUInt32(
            const std::initializer_list<const UTF8*>& in_rTokens) override final
        {
            return AssetDataJson::VGetUInt32(in_rTokens);
        }

        virtual Float32 VGetFloat32(
            const std::initializer_list<const UTF8*>& in_rTokens) override final
        {
            return AssetDataJson::VGetFloat32(in_rTokens);
        }

        virtual Core::Common::FixString1024 VGetChar(
            const std::initializer_list<const UTF8*>& in_rTokens) override final
        {
            return AssetDataJson::VGetChar(in_rTokens);
        }

    protected:
        virtual Bool _VLoad(::Platform::FileInterface&) override final;
        virtual void _VUnload() override final;

    private:
        Uint32 _version = 0;
    };
}  // namespace Game::Asset
