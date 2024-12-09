#pragma once

#include "Engine/Common/CustomString.h"
#include "Engine/File/Path.h"

// エンジンの最小インクルード
#include "Engine/MiniEngine.h"

// 前方宣言
namespace Platform
{
    class FileInterface;
}

namespace AssetManager
{
    /// <summary>
    /// アセットデータの基本
    /// </summary>
    class AssetDataBase
    {
        HE_CLASS_COPY_NG(AssetDataBase);
        HE_CLASS_MOVE_NG(AssetDataBase);

        friend class AssetManagerModule;

    public:
        AssetDataBase()          = default;
        virtual ~AssetDataBase() = default;

    protected:
        virtual void _VInit(const HE::Char* in_szName, const Core::File::Path& in_rPath);
        virtual HE::Bool _VLoad(Platform::FileInterface&) = 0;
        virtual void _VUnload()                           = 0;

    protected:
        Core::Common::FixedString128 _szName;
        Core::File::Path _path;
    };

    /// <summary>
    /// アセットデータのTreeノード型への各プロパティを取得インターフェイス
    /// </summary>
    class AssetDataWithTreeNodePropertyInterface
    {
    public:
        // 指定するノードのトークン名はアルファベットと数値のみなのでUTF8型にした
        virtual HE::Uint32 VGetUInt32(const std::initializer_list<const HE::UTF8*>&)   = 0;
        virtual HE::Sint32 VGetSInt32(const std::initializer_list<const HE::UTF8*>&)   = 0;
        virtual HE::Float32 VGetFloat32(const std::initializer_list<const HE::UTF8*>&) = 0;
        virtual Core::Common::FixedString1024 VGetChar(
            const std::initializer_list<const HE::UTF8*>&) = 0;
    };

}  // namespace AssetManager
