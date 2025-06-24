#include "AssetManager/AssetDataBinary.h"

// 依存モジュール
#include "Engine/Platform/PlatformFile.h"

namespace AssetManager
{
    HE::Bool AssetManager::AssetDataBinary::_VLoad(Platform::FileInterface& in_rFile)
    {
        // ファイルをバイナル形式でロード
        auto [pData, _uSize] = in_rFile.VLoadBinary(this->_path);
        this->_pData         = pData;
        this->_uSize         = _uSize;

        return (this->_pData != NULL);
    }

    void AssetDataBinary::_VUnload()
    {
        // バイナリでロードしたファイルを解放
        HE_SAFE_DELETE_MEM(this->_pData);
        this->_uSize = 0;
    }

}  // namespace AssetManager
