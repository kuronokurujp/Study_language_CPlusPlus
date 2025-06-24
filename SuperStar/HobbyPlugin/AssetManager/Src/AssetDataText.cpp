#include "AssetManager/AssetDataText.h"

// 依存モジュール
#include "Engine/Platform/PlatformFile.h"

namespace AssetManager
{
    HE::Bool AssetManager::AssetDataText::_VLoad(Platform::FileInterface& in_rFile)
    {
        // ファイルをテキストでロード
        auto [pData, _uSize] = in_rFile.VLoadText(this->_path);
        this->_pText         = reinterpret_cast<HE::Char*>(pData);
        this->_uSize         = _uSize;

        return (this->_pText != NULL);
    }

    void AssetDataText::_VUnload()
    {
        // バイナリでロードしたファイルを解放
        HE_SAFE_DELETE_MEM(this->_pText);
        this->_uSize = 0;
    }

}  // namespace AssetManager
