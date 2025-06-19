#include "Engine/Math/Math.h"

namespace Core::Math
{
    Vector3 TransformByVector3(const Vector3& in_rVec3, const Quaternion& in_rQuaternion)
    {
        // ベクトルをクォータニオンで計算
        Vector3 qv(in_rQuaternion._fX, in_rQuaternion._fY, in_rQuaternion._fZ);

        Vector3 retV = Vector3();

        {
            Vector3 temp(in_rVec3);
            Vector3 temp2 =
                Vector3::Cross(qv, Vector3::Cross(qv, in_rVec3) + temp * in_rQuaternion._fW);

            // 2.0fを-2.0fにすると90度に回転したベクトルになる
            // 反射ベクトルに使えそう
            auto vec3 = temp2 * 2.0f;

            retV.SetAdd(in_rVec3, vec3);
        }

        return retV;
    }

    /// <summary>
    /// Matrix4s the form quaternion.
    /// </summary>
    void OutputQuaternionToMatrix4(Matrix4* out, const Quaternion& in_rQuaternion)
    {
        auto q = in_rQuaternion;

        // 1行n列
        out->_faMat[0][0] = 1.0f - 2.0f * q._fY * q._fY - 2.0f * q._fZ * q._fZ;
        out->_faMat[0][1] = 2.0f * q._fX * q._fY + 2.0f * q._fW * q._fZ;
        out->_faMat[0][2] = 2.0f * q._fX * q._fZ - 2.0f * q._fW * q._fY;
        out->_faMat[0][3] = 0.0f;

        // 2行n列
        out->_faMat[1][0] = 2.0f * q._fX * q._fY - 2.0f * q._fW * q._fZ;
        out->_faMat[1][1] = 1.0f - 2.0f * q._fX * q._fX - 2.0f * q._fZ * q._fZ;
        out->_faMat[1][2] = 2.0f * q._fY * q._fZ + 2.0f * q._fW * q._fX;
        out->_faMat[1][3] = 0.0f;

        // 3行n列
        out->_faMat[2][0] = 2.0f * q._fX * q._fZ + 2.0f * q._fW * q._fY;
        out->_faMat[2][1] = 2.0f * q._fY * q._fZ - 2.0f * q._fW * q._fX;
        out->_faMat[2][2] = 1.0f - 2.0f * q._fX * q._fX - 2.0f * q._fY * q._fY;
        out->_faMat[2][3] = 0.0f;

        // 4行n列
        out->_faMat[3][0] = 0;
        out->_faMat[3][1] = 0;
        out->_faMat[3][2] = 0;
        out->_faMat[3][3] = 1.0f;
    }
}  // namespace Core::Math
