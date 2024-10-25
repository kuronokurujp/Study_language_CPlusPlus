#pragma once

#include "Vector3.h"

namespace Core::Math
{
    class Quaternion final
    {
    public:
        explicit Quaternion();
        explicit Quaternion(const Float32 in_fX, const Float32 in_fY, const Float32 in_fZ,
                            const Float32 in_fW);
        explicit Quaternion(const Vector3& in_rAxis, const Float32 in_fAngle);

        /// <summary>
        /// Sets the specified in x.
        /// </summary>
        void Set(const Float32 in_fX, const Float32 in_fY, const Float32 in_fZ,
                 const Float32 in_fW);

        /// <summary>
        /// Angles the specified in in r angle.
        /// </summary>
        void AngleUnitRadian(const Vector3& in_rAngle);

        /// <summary>
        /// Concatenates the specified in q.
        /// </summary>
        static Quaternion Concatenate(const Quaternion& in_fQ, const Quaternion& in_fP);

        static const Quaternion Identity;

    public:
        Float32 _fX = 0.0f, _fY = 0.0f, _fZ = 0.0f, _fW = 0.0f;
    };
}  // namespace Core::Math
