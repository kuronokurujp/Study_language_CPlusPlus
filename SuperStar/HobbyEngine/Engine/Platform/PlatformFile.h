#pragma once

#include "Engine/Common/Handle.h"
#include "Engine/Core.h"
#include "Engine/File/Path.h"

// プラットフォームのファイルシステムインターフェイス

namespace Platform
{
    /// <summary>
    /// ファイルシステム管理のインターフェイス
    /// </summary>
    class FileInterface
    {
    public:
        virtual ~FileInterface() = default;

        virtual void SetCurrentDir(const Core::File::Path&) = 0;

        virtual const Core::Common::Handle VFileOpen(const Core::File::Path& in_rPath) = 0;
        virtual HE::Bool VFileClose(const Core::Common::Handle& in_rHandle)            = 0;
        virtual HE::Uint32 VFileRead(void* out_pBuff, const Core::Common::Handle& in_rHandle,
                                     const HE::Uint32 in_uSize)                        = 0;

        virtual HE::Sint32 VFileSize(const Core::Common::Handle& in_rHandle) = 0;
        // TODO: ファイルを開いて一気にバイナルデータを出力するのが必要かも
        // ファイルを開いてバイナルデータを返す
        virtual std::tuple<void*, HE::Uint32> VLoadBinary(const Core::File::Path&) = 0;
        // テキストファイルをロードしてテキストデータを出力
        virtual std::tuple<HE::Char*, HE::Uint32> VLoadText(const Core::File::Path&) = 0;
    };
}  // namespace Platform
