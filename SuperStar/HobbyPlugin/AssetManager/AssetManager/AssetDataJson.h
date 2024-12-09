#pragma once

#include "AssetDataBase.h"
#include "Engine/Memory/Memory.h"

// TODO: ヘッダーにOSSのヘッダーファイルをインクルードしない方がいい気がする
// 他のプラグインからインクルードするとそのプラグインからもOSSの機能が使えてしまうから
#include "AssetManager/ThirdParty/simidjson/simdjson.h"

// エンジンの最小インクルード
#include "Engine/MiniEngine.h"

namespace AssetManager
{
    /// <summary>
    /// jsonアセットデータ
    /// </summary>
    class AssetDataJson : public AssetDataBase, public AssetDataWithTreeNodePropertyInterface
    {
        HE_CLASS_COPY_NG(AssetDataJson);
        HE_CLASS_MOVE_NG(AssetDataJson);

    public:
        using OutputJsonValue = simdjson::fallback::ondemand::value;

    public:
        AssetDataJson() : AssetDataBase() {}
        virtual ~AssetDataJson() = default;

        // 指定するノードのトークン名はアルファベットと数値のみなのでUTF8型にした
        virtual HE::Uint32 VGetUInt32(const std::initializer_list<const HE::UTF8*>&) override;
        virtual HE::Sint32 VGetSInt32(const std::initializer_list<const HE::UTF8*>&) override;
        virtual HE::Float32 VGetFloat32(const std::initializer_list<const HE::UTF8*>&) override;
        virtual Core::Common::FixedString1024 VGetChar(
            const std::initializer_list<const HE::UTF8*>&) override;

        /// <summary>
        /// 指定したトークンが存在するか
        /// </summary>
        HE::Bool IsToken(const std::initializer_list<const HE::UTF8*>&);

    protected:
        virtual HE::Bool _VLoad(Platform::FileInterface&) override;
        virtual void _VUnload() override;

    protected:
        Core::Common::Handle _fileHandle;

        Core::Memory::UniquePtr<simdjson::padded_string> _json;
        Core::Memory::UniquePtr<simdjson::ondemand::parser> _parser;
        void* _pDoc = NULL;
    };
}  // namespace AssetManager
