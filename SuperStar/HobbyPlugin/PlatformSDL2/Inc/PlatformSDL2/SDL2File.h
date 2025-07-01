#pragma once

// SDL2のファイルシステム
#include "Engine/Common/CustomMap.h"
#include "Engine/File/Path.h"
#include "Engine/MiniEngine.h"
#include "Engine/Platform/PlatformFile.h"

namespace PlatformSDL2
{
    /// <summary>
    /// ファイルシステム
    /// </summary>
    class File final : public Platform::FileInterface
    {
    public:
        using BinaryData = std::tuple<void*, HE::Uint32>;

    public:
        void SetCurrentDir(const Core::File::Path&);

        virtual const Core::Common::Handle VFileOpen(const Core::File::Path& in_rPath) override;
        HE::Bool VFileClose(const Core::Common::Handle&) override final;

        HE::Uint32 VFileRead(void* out_pBuff, const Core::Common::Handle& in_rHandle,
                             const HE::Uint32 in_size) override final;
        HE::Sint32 VFileSize(const Core::Common::Handle&) override final;

        // TODO: ファイルを開いて一気にバイナルデータを出力するのが必要かも
        // ファイルを開いてバイナルヒープを返す
        BinaryData VLoadBinary(const Core::File::Path&) override final;
        // テキストファイルをロードしてテキストデータを出力
        std::tuple<HE::Char*, HE::Uint32> VLoadText(const Core::File::Path&) override final;

    private:
        /// <summary>
        /// ファイルロードしたハンドルマップ
        /// </summary>
        Core::Common::FixedMap<Core::Common::Handle, struct FileInfo*, 2048> _mFileHandle;
        Core::File::Path _currentDirPath;
    };
}  // namespace PlatformSDL2
