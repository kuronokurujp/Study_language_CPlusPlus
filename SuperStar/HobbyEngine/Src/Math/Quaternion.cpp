#include "Engine/Math/Quaternion.h"

#include "Engine/Math/MathCore.h"

namespace Core::Math
{
    // 行列が行優先前提で処理している
    const Quaternion Quaternion::Identity(0.0f, 0.0f, 0.0f, 1.0f);

    Quaternion::Quaternion()
    {
        *this = Identity;
    }

    Quaternion::Quaternion(const HE::Float32 in_fX, const HE::Float32 in_fY,
                           const HE::Float32 in_fZ, const HE::Float32 in_fW)
    {
        this->Set(in_fX, in_fY, in_fZ, in_fW);
    }

    Quaternion::Quaternion(const Vector3& in_rAxis, const HE::Float32 in_fAngle)
    {
        const HE::Float32 scaler = Sin(in_fAngle * 0.5f);
        this->_fX                = in_rAxis._fX * scaler;
        this->_fY                = in_rAxis._fY * scaler;
        this->_fZ                = in_rAxis._fZ * scaler;

        this->_fW = Cos(in_fAngle * 0.5f);
    }

    void Quaternion::AngleUnitRadian(const Vector3& in_rAngle)
    {
        // オイラー角をクォータニオンとして設定
        const HE::Float32 fCosXAxis = Cos(in_rAngle._fX * 0.5f);
        const HE::Float32 fSinXAxis = Sin(in_rAngle._fX * 0.5f);
        const HE::Float32 fCosYAxis = Cos(in_rAngle._fY * 0.5f);
        const HE::Float32 fSinYAxis = Sin(in_rAngle._fY * 0.5f);
        const HE::Float32 fCosZAxis = Cos(in_rAngle._fZ * 0.5f);
        const HE::Float32 fSinZAxis = Sin(in_rAngle._fZ * 0.5f);

        // ローカル座標系からワールド座標系への変換
        this->_fW = fCosYAxis * fCosXAxis * fCosZAxis + fSinYAxis * fSinXAxis * fSinZAxis;
        this->_fX = fCosYAxis * fSinXAxis * fCosZAxis + fSinYAxis * fCosXAxis * fSinZAxis;
        this->_fY = fSinYAxis * fCosXAxis * fCosZAxis - fCosYAxis * fSinXAxis * fSinZAxis;
        this->_fZ = fCosYAxis * fCosXAxis * fSinZAxis - fSinYAxis * fSinXAxis * fCosZAxis;
    }

    void Quaternion::Set(const HE::Float32 in_fX, const HE::Float32 in_fY, const HE::Float32 in_fZ,
                         const HE::Float32 in_fW)
    {
        this->_fX = in_fX;
        this->_fY = in_fY;
        this->_fZ = in_fZ;
        this->_fW = in_fW;
    }

    /// <summary>
    /// Concatenates the specified in q.
    /// </summary>
    Quaternion Quaternion::Concatenate(const Quaternion& in_rQ, const Quaternion& in_rP)
    {
        Quaternion retVal;

        Vector3 pv(in_rP._fX, in_rP._fY, in_rP._fZ);
        Vector3 qv(in_rQ._fX, in_rQ._fY, in_rQ._fZ);

        const Vector3 newVec = (qv * in_rP._fW) + (pv * in_rQ._fW) + Vector3::Cross(pv, qv);
        retVal._fX           = newVec._fX;
        retVal._fY           = newVec._fY;
        retVal._fZ           = newVec._fZ;

        retVal._fW = in_rP._fW * in_rQ._fW - Vector3::Dot(pv, qv);

        return retVal;
    }
}  // namespace Core::Math
