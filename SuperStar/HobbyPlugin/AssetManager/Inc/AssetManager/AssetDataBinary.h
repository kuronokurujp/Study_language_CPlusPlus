#pragma once

#include "AssetManager/AssetDataBase.h"

namespace AssetManager
{
    /// <summary>
    /// アセットデータの基本
    /// </summary>
    class AssetDataBinary final : public AssetDataBase
    {
        HE_CLASS_COPY_NG(AssetDataBinary);
        HE_CLASS_MOVE_NG(AssetDataBinary);

        friend class AssetManagerModule;

    public:
        AssetDataBinary() : AssetDataBase() {}

        /// <summary>
        /// ロードしたデータメモリを取得
        /// ヒープ確報しているので解放とかしてはいけない
        /// </summary>
        void* Mem() const { return this->_pData; }
        // データサイズ
        HE::Uint32 Size() const { return this->_uSize; }

    protected:
        HE::Bool _VLoad(Platform::FileInterface&) override final;
        void _VUnload() override final;

    private:
        void* _pData      = NULL;
        HE::Uint32 _uSize = 0;
    };

}  // namespace AssetManager
