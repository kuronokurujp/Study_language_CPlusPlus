#include "Engine/Math/Vector2.h"

namespace Core::Math
{
    const Vector2 Vector2::Zero = Vector2();

    HE::Bool Vector2::IsZero() HE_NOEXCEPT
    {
        if (this->_fX != 0.0f) return FALSE;
        if (this->_fY != 0.0f) return FALSE;

        return TRUE;
    }

    /// <summary>
    /// スケールしたベクトルを加算
    /// </summary>
    void Vector2::Madd(const Vector2& in_rBase, const HE::Float32 in_fScale) HE_NOEXCEPT
    {
        this->_fX += (in_rBase._fX * in_fScale);
        this->_fY += (in_rBase._fY * in_fScale);
    }

    /// <summary>
    /// Sets the madd.
    /// </summary>
    void Vector2::SetMadd(const Vector2& in_rOffset, const HE::Float32 in_fScale,
                          const Vector2& in_rBase) HE_NOEXCEPT
    {
        this->_fX = in_rOffset._fX * in_fScale + in_rBase._fX;
        this->_fY = in_rOffset._fY * in_fScale + in_rBase._fY;
    }

    /// <summary>
    /// Sets the add.
    /// </summary>
    void Vector2::SetAdd(const Vector2& in_rLeft, const Vector2& in_rRight) HE_NOEXCEPT
    {
        this->_fX = in_rLeft._fX + in_rRight._fX;
        this->_fY = in_rLeft._fY + in_rRight._fY;
    }

    /// <summary>
    /// Sets the sub.
    /// </summary>
    void Vector2::SetSub(const Vector2& in_rLeft, const Vector2& in_rRight) HE_NOEXCEPT
    {
        this->_fX = in_rLeft._fX - in_rRight._fX;
        this->_fY = in_rLeft._fY - in_rRight._fY;
    }

    /// <summary>
    /// Sets the mul.
    /// </summary>
    void Vector2::SetMul(const Vector2& in_rLeft, const Vector2& in_rRight) HE_NOEXCEPT
    {
        this->_fX = in_rLeft._fX * in_rRight._fX;
        this->_fY = in_rLeft._fY * in_rRight._fY;
    }

    /// <summary>
    /// ２次元補間を行う
    /// </summary>
    void Vector2::SetInterporation(const Vector2& in_rBase, const Vector2& in_rULine,
                                   const Vector2& in_rVLIne, const HE::Float32 in_fUScale,
                                   const HE::Float32 in_fVScale) HE_NOEXCEPT
    {
        // a + u(b - a)
        this->SetMadd(in_rULine, in_fUScale, in_rBase);
        // a + u(b - a) + v(c - a)
        this->Madd(in_rVLIne, in_fVScale);
    }

    /// <summary>
    /// 正規化
    /// </summary>
    void Vector2::Normalize() HE_NOEXCEPT
    {
        const HE::Float32 fMagSq = this->_fX * this->_fX + this->_fY * this->_fY;
        if (fMagSq > 0.f)
        {
            const HE::Float32 fNewOverMag = 1.0f / sqrtf(fMagSq);
            this->_fX *= fNewOverMag;
            this->_fY *= fNewOverMag;
        }
    }
}  // namespace Core::Math
