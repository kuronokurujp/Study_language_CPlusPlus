#pragma once

#include "Engine/Common/Handle.h"
#include "Engine/Common/PoolManager.h"
#include "Engine/File/Path.h"
#include "Engine/MiniEngine.h"
#include "Engine/Module/Module.h"

// モジュールのヘッダーファイルは全てインクルードする
#include "AssetManager/AssetDataBase.h"
#include "AssetManager/AssetDataBinary.h"
#include "AssetManager/AssetDataJson.h"
#include "AssetManager/AssetDataText.h"
#include "AssetManager/AssetDataToml.h"
#include "AssetManager/AssetDataXml.h"

namespace AssetManager
{
    // 前方宣言
    class AssetDataBase;

    /// <summary>
    /// エンジンのアセット対応のモジュール
    /// </summary>
    class AssetManagerModule final : public Module::ModuleBase
    {
        HE_MODULE_GENRATE_DECLARATION(AssetManagerModule);

    public:
        AssetManagerModule();

        template <class T>
        typename std::enable_if<std::is_base_of<AssetDataBase, T>::value,
                                const Core::Common::Handle>::type
        Load(const HE::Char* in_pName, const Core::File::Path& in_rFilePath)
        {
            HE_ASSERT(in_rFilePath.Empty() == FALSE);
            HE_ASSERT(in_pName);
            // TODO: 名前が重複した時はどうするかは考える

            // インスタンスを確保
            auto [handle, p] = this->_poolAssetDataManager.Alloc<T>();

            AssetDataBase* pAsset = p;
            pAsset->_VInit(in_pName, in_rFilePath.Str());

            // アセットのロードをして失敗したら確保したインスタンスを解放
            if (this->_Load(pAsset))
            {
                return handle;
            }

            this->_poolAssetDataManager.Free(handle, TRUE);

            return NullHandle;
        }

        void Unload(const Core::Common::Handle&);

        /// <summary>
        /// ファイルパスからアセットハンドルを取得
        /// </summary>
        const Core::Common::Handle GetAssetHandle(const Core::File::Path& in_rFilePath);

        template <class T>
        typename std::enable_if<std::is_base_of<AssetDataBase, T>::value, T&>::type GetAsset(
            const Core::Common::Handle& in_rHandle)
        {
            T* p = reinterpret_cast<T*>(this->_poolAssetDataManager.Ref(in_rHandle));
            HE_ASSERT(p && "ロードしたアセットデータがない");
            return *p;
        }

        /// <summary>
        /// アセット格納ディレクトリ設定
        /// </summary>
        void SetCurrentDir(const Core::File::Path&);

    protected:
        /// <summary>
        /// モジュール開始
        /// </summary>
        HE::Bool _VStart() override final;

        /// <summary>
        /// モジュール解放
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        HE::Bool _VRelease() override final;

    private:
        HE::Bool _Load(AssetDataBase* out_pAssetData);

    private:
        Core::Common::RuntimePoolManager<AssetDataBase> _poolAssetDataManager;
    };
}  // namespace AssetManager
