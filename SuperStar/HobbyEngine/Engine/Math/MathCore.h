#pragma once

#include <cmath>

namespace Core::Math
{
    // 円周率
    constexpr HE::Float32 fPI                = 3.14159265359f;
    constexpr HE::Float32 fTwoPI             = 2.0f * fPI;
    constexpr HE::Float32 fPIOver2           = fPI / 2.0f;
    constexpr HE::Float32 fRadiansUnit       = fPI / 180.0f;
    constexpr HE::Float32 fInvertRadiansUnit = 180.0f / fPI;
    constexpr HE::Float32 fInvert255         = 1.0f / 255.0f;

    /// <summary>
    /// 基準位置タイプ
    /// </summary>
    enum EAnchor
    {
        EAnchor_Left = 0,
        EAnchor_Center,
    };

    // 2乗の値に整形する関数
    // 負数は未対応
    static inline HE::Uint32 PowerOfTwo(const HE::Uint32 value)
    {
        if (value == 0) return 0;

        HE::Uint32 a = 2;
        while (a < value)
        {
            a *= 2;
        }

        return a;
    }

    /// <summary>
    /// Radianses to degrees.
    /// </summary>
    static inline HE::Float32 RadiansToDegrees(HE::Float32 in_fRadians) HE_NOEXCEPT
    {
        return fInvertRadiansUnit * in_fRadians;
    }

    /// <summary>
    /// Degreeses to radians.
    /// </summary>
    static inline HE::Float32 DegreesToRadians(HE::Float32 in_fDegrees) HE_NOEXCEPT
    {
        return fRadiansUnit * in_fDegrees;
    }

    /// <summary>
    /// Sins the specified in angle.
    /// </summary>
    static inline HE::Float32 Sin(const HE::Float32 in_fAngle) HE_NOEXCEPT
    {
        return sinf(in_fAngle);
    }

    /// <summary>
    /// Coses the specified in angle.
    /// </summary>
    static inline HE::Float32 Cos(const HE::Float32 in_fAngle) HE_NOEXCEPT
    {
        return cosf(in_fAngle);
    }

    /// <summary>
    /// Tans the specified in angle.
    /// </summary>
    static inline HE::Float32 Tan(const HE::Float32 in_fAngle) HE_NOEXCEPT
    {
        return tanf(in_fAngle);
    }

    /// <summary>
    /// Acoses the specified in value.
    /// </summary>
    static inline HE::Float32 Acos(const HE::Float32 in_fValue) HE_NOEXCEPT
    {
        return acosf(in_fValue);
    }

    /// <summary>
    /// Atan2s the specified in x.
    /// </summary>
    static inline HE::Float32 Atan2(const HE::Float32 in_fX, const HE::Float32 in_fY) HE_NOEXCEPT
    {
        return atan2f(in_fY, in_fX);
    }

    static inline HE::Float32 Cot(const HE::Float32 in_fAngle)
    {
        return 1.0f / Tan(in_fAngle);
    }

    /// <summary>
    /// 値が直近0かどうか.
    /// </summary>
    static inline HE::Bool NearZero(const HE::Float32 in_fValue,
                                    const HE::Float32 in_fEpsilon = 0.001f) HE_NOEXCEPT
    {
        if (fabsf(in_fValue) <= in_fEpsilon)
        {
            return TRUE;
        }

        return FALSE;
    }

    /// <summary>
    /// Rounds the f.
    /// </summary>
    static inline HE::Float32 RoundF(const HE::Float32 in_fValue) HE_NOEXCEPT
    {
        // 四捨五入する時に+/-でずらす方向を変えている
        HE::Float32 fOffset = (in_fValue >= 0.0f) ? 0.5f : -0.5f;
        return static_cast<HE::Float32>(static_cast<HE::Uint32>(in_fValue + fOffset));
    }

    /// <summary>
    /// Rounds the specified in r value.
    /// </summary>
    static inline HE::Uint32 Round(const HE::Float32 in_fValue) HE_NOEXCEPT
    {
        return static_cast<HE::Uint32>(RoundF(in_fValue));
    }

    /// <summary>
    /// 値と最大値を設定して値を0-1の範囲値にする
    /// </summary>
    static inline HE::Float32 Range0to1(const HE::Uint32 in_uSrc,
                                        const HE::Uint32 in_uMax) HE_NOEXCEPT
    {
        if (in_uSrc <= 0) return 0.0f;
        auto f = static_cast<HE::Float32>(in_uSrc) / static_cast<HE::Float32>(in_uMax);
        if (1.0f < f) return 1.0f;
        return f;
    }

}  // namespace Core::Math
