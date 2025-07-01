#include "Engine/Common/Handle.h"

namespace Core::Common
{
    // Uint32の最大数は42億になるので,
    // インデックスとマジックナンバーの組み合わせでかぶる事はないと思う
    static HE::Uint32 s_uMagicNumber = 0;

    /// <summary>
    /// 管理するindexを設定
    /// </summary>
    void Handle::SetIndex(const HE::Uint32 in_uIndex)
    {
        // TODO: 絶対枯渇しない方法を考える必要はあるだろうか?
        ++s_uMagicNumber;
        this->SetIndex(in_uIndex, s_uMagicNumber);
    }

    void Handle::SetIndex(const HE::Uint32 in_uIndex, const HE::Uint32 in_uMagicNumber)
    {
        HE_ASSERT(this->Null());
        HE_ASSERT(in_uIndex <= EMax_Index);

        // インデックスとマジックナンバーを割り当て
        this->_handleField._uIndex = in_uIndex;
        this->_handleField._uMagic = in_uMagicNumber;
    }
}  // namespace Core::Common

const Core::Common::Handle NullHandle;
