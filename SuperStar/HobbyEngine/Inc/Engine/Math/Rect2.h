#pragma once

#include "Engine/Core.h"
#include "MathCore.h"
#include "Vector2.h"

/// <summary>
/// 原点中心
/// 右手座標系
/// </summary>
namespace Core::Math::RC
{
    /// <summary>
    /// 2Dの矩形クラス
    /// </summary>
    class Rect2D final
    {
    public:
        /// <summary>
        /// 原点が左上など中心以外で作成した矩形情報から中心矩形に変換して出力
        /// パラメータはEAnchorで指定されたアンカーを基準にしている
        /// コピーコンストラクタは許容
        /// </summary>
        static Rect2D ConvertFromAnchor(const Vector2& in_rPos, const Vector2& in_rSize,
                                        const EAnchor);

    public:
        Rect2D() { this->Clear(); }
        Rect2D(const HE::Float32 in_fX, const HE::Float32 in_fY, const HE::Float32 in_fW,
               const HE::Float32 in_fH);

        void Set(const HE::Float32 in_fX, const HE::Float32 in_fY, const HE::Float32 in_fW,
                 const HE::Float32 in_fH);

        /// <summary>
        /// Gets the position.
        /// </summary>
        const Vector2 GetPos() const;
        inline const Vector2 GetSize() const { return Vector2(this->_fW, this->_fH); }

        /// <summary>
        /// Gets the Width
        /// </summary>
        inline HE::Float32 Width() const HE_NOEXCEPT { return this->_fW; }

        /// <summary>
        /// Widthes the half.
        /// </summary>
        inline HE::Float32 WidthHalf() const HE_NOEXCEPT { return (this->Width() * 0.5f); }

        /// <summary>
        /// Heights this instance.
        /// </summary>
        inline HE::Float32 Height() const HE_NOEXCEPT { return this->_fH; }

        /// <summary>
        /// Heights the half.
        /// </summary>
        inline HE::Float32 HeightHalf() const HE_NOEXCEPT { return (this->Height() * 0.5f); }

        /// <summary>
        /// 矩形同士が交差しているかどうか
        /// 矩形の縁の交差も交差として扱う
        /// </summary>
        HE::Bool IsRectIntersect(const Rect2D& in_rOrderRect) const;

        /// <summary>
        /// 座標が矩形の中に入っているか
        /// 矩形の縁に触れた場合も中に入っていると扱う
        /// </summary>
        HE::Bool IsPointInside(const Vector2& in_rPos) const;

        /// <summary>
        /// Clears this instance.
        /// </summary>
        inline void Clear() HE_NOEXCEPT
        {
            this->_fX = 0.0f;
            this->_fY = 0.0f;
            this->_fW = 0.0f;
            this->_fH = 0.0f;
        }

        /// <summary>
        /// Operator=s the specified a.
        /// </summary>
        inline void operator=(const Vector2& in_rVec) HE_NOEXCEPT
        {
            this->_fX = in_rVec._fX;
            this->_fY = in_rVec._fY;
        }

        /// <summary>
        /// Operator+=s the specified a.
        /// </summary>
        inline void operator+=(const Vector2& in_rVec) HE_NOEXCEPT
        {
            this->_fX += in_rVec._fX;
            this->_fY += in_rVec._fY;
        }

        HE::Float32 _fX = 0.0f;
        HE::Float32 _fY = 0.0f;
        HE::Float32 _fW = 0.0f;
        HE::Float32 _fH = 0.0f;
    };
}  // namespace Core::Math::RC
