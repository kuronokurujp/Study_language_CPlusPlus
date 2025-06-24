#include "AssetManager/AssetDataBase.h"

#include <fstream>

namespace AssetManager
{
    void AssetDataBase::_VInit(const HE::Char* in_szName, const Core::File::Path& in_rPath)
    {
        HE_ASSERT(in_szName && "名前が存在しない");
        HE_ASSERT(in_rPath.Empty() == FALSE);

        this->_szName = in_szName;
        this->_path   = in_rPath;
    }

}  // namespace AssetManager
