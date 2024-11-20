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

        // TODO: パラメータを指定して取得

    protected:
        virtual Bool _VLoad(::Platform::FileInterface&) override;

    private:
        Uint32 _version = 0;
    };
}  // namespace Game::Asset
