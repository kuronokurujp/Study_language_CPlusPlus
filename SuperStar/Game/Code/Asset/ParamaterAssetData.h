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
        inline Uint32 Version() const { return this->_version; }

        // 指定名はアルファベットと数値のみなのでUTF8型にした
        Uint32 GetUInt32ByIdData(const UTF8* in_pIdName, const UTF8* in_pFieldName);
        Float32 GetFloat32ByIdData(const UTF8* in_pIdName, const UTF8* in_pFieldName);
        /*
        const Core::Common::StringBase&& GetCharByIdData(const UTF8* in_pIdName, const UTF8* in_pFieldName);
        */

    protected:
        virtual Bool _VLoad(::Platform::FileInterface&) override final;
        virtual void _VUnload() override final;

    private:
        Uint32 _version = 0;
        void* _pData = NULL;
    };
}  // namespace Game::Asset
