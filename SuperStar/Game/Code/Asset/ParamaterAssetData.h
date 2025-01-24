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
        void OutputStringByIdData(Core::Common::StringBase* out, const HE::UTF8* in_pIdName,
                                  const HE::UTF8* in_pFieldName);

        HE::Bool IsId(const HE::UTF8* in_pIdName);

    protected:
        virtual HE::Bool _VLoad(::Platform::FileInterface&) override final;
        virtual void _VUnload() override final;

    private:
        HE::Uint32 _version = 0;
    };
}  // namespace Game::Asset
