#include "ParamaterAssetData.h"

namespace Game::Asset
{
    Bool ParamaterAssetData::_VLoad(Platform::FileInterface& in_rFileSystem)
    {
        if (AssetManager::AssetDataJson::_VLoad(in_rFileSystem) == FALSE) return FALSE;

        // バージョン値を取得
        OutputJsonValue v;
        this->_OutputValue(&v, HE_STR_TEXT("version"));
        this->_version = v.get_uint64();

        return TRUE;
    }
}  // namespace Game::Asset
