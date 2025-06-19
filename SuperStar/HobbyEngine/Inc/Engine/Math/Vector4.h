#pragma once

#include "Engine/Core.h"

namespace Core::Math
{
    class Vector4 final
    {
    public:
        explicit Vector4() : _fX(0.0f), _fY(0.0f), _fZ(0.0f), _fW(0.0f) {}
        explicit Vector4(const HE::Float32 in_fX, const HE::Float32 in_fY, const HE::Float32 in_fZ,
                         const HE::Float32 in_fW)
            : _fX(in_fX), _fY(in_fY), _fZ(in_fZ), _fW(in_fW)
        {
        }

        Vector4 Mul(const Vector4& in_rSrc)
        {
            const HE::Float32 outputX = this->_fX * in_rSrc._fX;
            const HE::Float32 outputY = this->_fY * in_rSrc._fY;
            const HE::Float32 outputZ = this->_fZ * in_rSrc._fZ;
            const HE::Float32 outputW = this->_fW * in_rSrc._fW;

            return Vector4(outputX, outputY, outputZ, outputW);
        }

        HE::Float32 AddMul(const Vector4& in_rSrc)
        {
            const HE::Float32 outputX = this->_fX * in_rSrc._fX;
            const HE::Float32 outputY = this->_fY * in_rSrc._fY;
            const HE::Float32 outputZ = this->_fZ * in_rSrc._fZ;
            const HE::Float32 outputW = this->_fW * in_rSrc._fW;

            return (outputX + outputY + outputZ + outputW);
        }

        /// <summary>
        /// ベクトル要素をfloatポインタで返す.
        /// </summary>
        const HE::Float32* GetAsFloatPtr() const HE_NOEXCEPT { return &this->_fX; }

    public:
        // 頻繁にアクセスする変数にはprivate指定にはしない
        HE::Float32 _fX = 0.0f, _fY = 0.0f, _fZ = 0.0f, _fW = 0.0f;
    };
}  // namespace Core::Math
