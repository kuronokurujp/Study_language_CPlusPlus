#pragma once

#include "AssetManager/AssetDataBase.h"

namespace AssetManager
{
    /// <summary>
    /// テキストアセットデータの基本
    /// </summary>
    class AssetDataText final : public AssetDataBase
    {
        HE_CLASS_COPY_NG(AssetDataText);
        HE_CLASS_MOVE_NG(AssetDataText);

        friend class AssetManagerModule;

    public:
        AssetDataText() : AssetDataBase() {}

        /// <summary>
        /// ロードしたデータメモリを取得
        /// ヒープ確報しているので解放とかしてはいけない
        /// </summary>
        const HE::UTF8* Text() const { return this->_pText; }
        // データサイズ
        HE::Uint32 Size() const { return this->_uSize; }

    protected:
        HE::Bool _VLoad(Platform::FileInterface&) override final;
        void _VUnload() override final;

    private:
        HE::Char* _pText  = NULL;
        HE::Uint32 _uSize = 0;
    };

}  // namespace AssetManager
