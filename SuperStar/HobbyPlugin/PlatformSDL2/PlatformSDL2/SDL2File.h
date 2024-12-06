#pragma once

// DxLibのファイルシステム
#include "Engine/Common/CustomMap.h"
#include "Engine/File/Path.h"
#include "Engine/MiniEngine.h"
#include "Engine/Platform/PlatformFile.h"

namespace PlatformSDL2
{
    /// <summary>
    ///  DxLibのファイルシステム
    /// </summary>
    class File final : public Platform::FileInterface
    {
    public:
        virtual const Core::Common::Handle VFileOpen(const Core::File::Path& in_rPath) override;
        Bool VFileClose(const Core::Common::Handle&) override final;

        Bool VFileRead(void* out_pBuff, const Core::Common::Handle& in_rHandle,
                       const HE::HE::Uint32 in_size) override final;
        HE::HE::Uint32 VFileSize(const Core::Common::Handle&) override final;

    private:
        /// <summary>
        /// ファイルロードしたハンドルマップ
        /// </summary>
        Core::Common::FixedMap<Uint32, Core::Common::Handle, 2048> _mFileHandle;
    };
}  // namespace Platform::SDL2
