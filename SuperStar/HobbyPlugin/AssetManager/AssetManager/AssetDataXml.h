#pragma once

#include "AssetDataBase.h"

// TODO: ヘッダーにOSSのヘッダーファイルをインクルードしない方がいい気がする
// 他のプラグインからインクルードするとそのプラグインからもOSSの機能が使えてしまうから
#include "AssetManager/ThirdParty/pugixml/pugixml.hpp"

// エンジンの最小インクルード
#include "Engine/MiniEngine.h"

namespace AssetManager
{
    /// <summary>
    /// xmlファイルを扱うアセットデータ
    /// </summary>
    class AssetDataXml : public AssetDataBase
    {
        HE_CLASS_COPY_NG(AssetDataXml);
        HE_CLASS_MOVE_NG(AssetDataXml);

    public:
        AssetDataXml() : AssetDataBase() {}
        virtual ~AssetDataXml() = default;

        virtual Bool _VLoad(Platform::FileInterface&) override;
        virtual void _VUnload() override;

    protected:
        Core::Common::Handle _fileHandle;
        pugi::xml_document _doc;
    };

}  // namespace AssetManager
