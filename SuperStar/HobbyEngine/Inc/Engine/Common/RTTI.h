#pragma once

#include "CustomString.h"
#include "Engine/Core.h"

namespace Core::Common
{
    /// <summary>
    /// カスタムRTTI
    /// </summary>
    class RTTI final
    {
        HE_CLASS_DEFAULT_CONSTRUCT_NG(RTTI);
        HE_CLASS_COPY_NG(RTTI);

    public:
        RTTI(const HE::Char* in_szClass) : _szClass(in_szClass), _pBaseRTTI(NULL)
        {
            HE_LOG_LINE(HE_STR_TEXT("RTTI BaseClassName: %s"), in_szClass);
        }

        RTTI(const HE::Char* in_szClass, const RTTI* in_pBaseRTTI)
            : _szClass(in_szClass), _pBaseRTTI(in_pBaseRTTI)
        {
            HE_LOG_LINE(HE_STR_TEXT("RTTI InheritClassName: %s BaseClassName: %s"), in_szClass,
                        in_pBaseRTTI->GetName().Str());
        }

        const StringBase& GetName() const HE_NOEXCEPT { return this->_szClass; }
        HE::Bool IsExactly(const RTTI& in_rRtti) const HE_NOEXCEPT
        {
            return (this->_pBaseRTTI == &in_rRtti);
        }

        HE::Bool DerivesFrom(const RTTI*) const;

    private:
        const FixedString128 _szClass;
        const RTTI* _pBaseRTTI = NULL;
    };
}  // namespace Core::Common
