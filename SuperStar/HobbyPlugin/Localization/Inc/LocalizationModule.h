#pragma once

#include "Engine/Common/CustomMap.h"
#include "Engine/File/Path.h"
#include "Engine/MiniEngine.h"
#include "Engine/Module/Module.h"

// 依存モジュール
#include "AssetManagerModule.h"

namespace Localization
{
    /// <summary>
    /// ローカライズのシステムアセットデータ
    /// </summary>
    class SystemAssetData final : public AssetManager::AssetDataToml
    {
    public:
        /// <summary>
        /// 言語のデータ
        /// </summary>
        struct LocateData
        {
        public:
            LocateData() {}
            LocateData(const LocateData& in_data) { this->_textFilePath = in_data._textFilePath; }

            LocateData(const Core::File::Path& in_rTextFileJsonPath)
            {
                this->_textFilePath = in_rTextFileJsonPath;
            }

            Core::File::Path _textFilePath;
        };

        using LocateMap =
            Core::Common::FixedMap<Core::Common::FixedString128, SystemAssetData::LocateData, 32>;

        /// <summary>
        /// ロケートマップデータを探して取得
        /// 常駐しているメンバー変数を参照するようにしている
        /// 呼び出し側が参照したメンバー変数の書き換えたをできないようにするためにconstにしている
        /// </summary>
        const LocateMap& FindLocate(const HE::Char*);

    protected:
        virtual HE::Bool _VLoad(Platform::FileInterface&) override;

    private:
        /// <summary>
        /// 各言語毎のデータ
        /// </summary>
        Core::Common::FixedMap<Core::Common::FixedString128, LocateMap, 32> _locateDataMap;
    };

    /// <summary>
    /// 言語のテキストアセットデータ
    /// </summary>
    class LocateAssetData final : public AssetManager::AssetDataJson
    {
    public:
        /// <summary>
        /// TODO: テキストサイズ
        /// </summary>
        const HE::Uint32 GetTextSize(const HE::UTF8*);

        /// <summary>
        /// TODO: テキストカラー
        /// </summary>
        const HE::Uint32 GetTextColor(const HE::UTF8*);

        /// <summary>
        /// テキスト
        /// </summary>
        const Core::Common::StringBase& GetText(const HE::UTF8*);

    private:
        // 一度読み込んだテキストをキャッシュするテキストバッファ
        // 同じデータを何度も読み込まないようにするため
        Core::Common::FixedMap<Core::Common::FixedString128, Core::Common::FixedString1024, 128>
            _textBuffMap;
    };

    /// <summary>
    /// 多言語対応のモジュール
    /// </summary>
    class LocalizationModule final : public Module::ModuleBase
    {
        HE_MODULE_GENRATE_DECLARATION(LocalizationModule);

    public:
        LocalizationModule();

        // ローカライズ設定データをロード(テキストファイル版)
        // バイナリファイル版も用意する
        HE::Bool LoadSystemFile(const Core::Common::StringBase&);
        // ローカライズ設定データをアンロード
        HE::Bool UnloadSystemFile();

        // ローカライズデータをロード
        HE::Bool LoadTextAll(const Core::Common::StringBase&);
        // ローカライズデータをアンロード
        HE::Bool UnloadTextAll(const Core::Common::StringBase&);

        // ローカライズテキストを取得
        const HE::Char* Text(const HE::Char* in_szLocateName, const HE::Char* in_szGroupName,
                             const HE::Char* in_szKey);

    protected:
        /// <summary>
        /// モジュール初期化
        /// </summary>
        /// <returns></returns>
        HE::Bool _VStart() override final;

        /// <summary>
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        HE::Bool _VRelease() override final;

    private:
        using LocateTextMap =
            Core::Common::FixedMap<Core::Common::FixedString128, Core::Common::Handle, 64>;

        Core::Common::Handle _sysAssetDataHandle;
        Core::Common::FixedMap<Core::Common::FixedString128, LocateTextMap, 16> _locateDataMap;
    };
}  // namespace Localization
