#pragma once

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
        virtual void _VInit(const Char* in_szName, const Core::File::Path& in_rPath);
        virtual Bool _VLoad(Platform::FileInterface&) = 0;
        virtual void _VUnload()                       = 0;

    protected:
        Core::Common::FixString128 _szName;
        Core::File::Path _path;
    };

}  // namespace AssetManager
