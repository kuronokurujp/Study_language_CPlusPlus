#pragma once

#include "Color.h"
#include "Engine/Core.h"
#include "MathCore.h"
#include "Matrix4.h"
#include "Quaternion.h"
#include "Rect2.h"
#include "Vector2.h"
#include "Vector3.h"

namespace Core::Math
{
    /// <summary>
    /// Vector3をクォータニオンに与えた結果を返す
    /// </summary>
    extern Vector3 TransformByVector3(const Vector3& in_rVec3, const Quaternion& in_rQuaternion);

    /// <summary>
    /// クォータニオンを行列に変えたのを返す
    /// </summary>
    extern void OutputQuaternionToMatrix4(Matrix4* out, const Quaternion& in_rQuaternion);

}  // namespace Core::Math
