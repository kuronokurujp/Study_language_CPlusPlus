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
        HE::Uint32 GetUInt32ByName(const HE::UTF8* in_pIdName, const HE::UTF8* in_pFieldName);
        HE::Sint32 GetSInt32ByName(const HE::UTF8* in_pIdName, const HE::UTF8* in_pFieldName);
        HE::Float32 GetFloat32ByName(const HE::UTF8* in_pIdName, const HE::UTF8* in_pFieldName);
        void OutputStringByName(Core::Common::StringBase* out, const HE::UTF8* in_pIdName,
                                const HE::UTF8* in_pFieldName);

        HE::Uint32 GetUInt32ByIndex(const HE::Uint32, const HE::UTF8*);
        HE::Sint32 GetSInt32ByIndex(const HE::Uint32, const HE::UTF8*);
        HE::Float32 GetFloat32ByIndex(const HE::Uint32, const HE::UTF8*);
        void OutputStringByIndex(Core::Common::StringBase* out, const HE::Uint32, const HE::UTF8*);
        void OutputIdByIndex(Core::Common::StringBase* out, const HE::Uint32);

        HE::Bool IsId(const HE::UTF8*);
        HE::Bool IsIndex(const HE::Uint32);

    protected:
        virtual HE::Bool _VLoad(::Platform::FileInterface&) override final;
        virtual void _VUnload() override final;

    private:
        HE::Uint32 _version = 0;
    };
}  // namespace Game::Asset
