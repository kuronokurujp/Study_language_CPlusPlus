#pragma once

#include <math.h>

#include "Engine/Core.h"

namespace Core::Math
{
    class Vector2 final
    {
    public:
        explicit Vector2() { this->Clear(); }

        // コントラクト
        explicit Vector2(const HE::Float32 in_fX, const HE::Float32 in_fY) : _fX(in_fX), _fY(in_fY)
        {
        }

        // コピーは代入文でできるようにするのがわかりやすいのでexplicitはつけない
        Vector2(const Vector2& in_v) { this->_fX = in_v._fX, this->_fY = in_v._fY; }
        Vector2(Vector2& in_v) { this->_fX = in_v._fX, this->_fY = in_v._fY; }
        Vector2(const HE::Float32 in_fV) { this->_fX = this->_fY = in_fV; }

        /// <summary>
        /// 0で初期化
        /// </summary>
        inline void Clear() HE_NOEXCEPT
        {
            this->_fX = 0.f;
            this->_fY = 0.f;
        }

        HE::Bool IsZero() HE_NOEXCEPT;

        /// <summary>
        /// スケールしたベクトルを加算
        /// </summary>
        void Madd(const Vector2& in_rBase, const HE::Float32 in_fScale) HE_NOEXCEPT;

        /// <summary>
        /// Sets the madd.
        /// </summary>
        void SetMadd(const Vector2& in_rOffset, const HE::Float32 in_fScale,
                     const Vector2& in_rBase) HE_NOEXCEPT;

        /// <summary>
        /// Sets the add.
        /// </summary>
        void SetAdd(const Vector2& in_rLeft, const Vector2& in_rRight) HE_NOEXCEPT;

        /// <summary>
        /// Sets the sub.
        /// </summary>
        void SetSub(const Vector2& in_rLeft, const Vector2& in_rRight) HE_NOEXCEPT;

        /// <summary>
        /// Sets the mul.
        /// </summary>
        void SetMul(const Vector2& in_rLeft, const Vector2& in_rRight) HE_NOEXCEPT;

        /// <summary>
        /// Sets the specified in value.
        /// </summary>
        inline void Set(const HE::Float32 in_fValue) HE_NOEXCEPT
        {
            this->_fX = this->_fY = in_fValue;
        }

        inline void Set(const HE::Float32 in_fX, const HE::Float32 in_fY) HE_NOEXCEPT
        {
            this->_fX = in_fX;
            this->_fY = in_fY;
        }

        /// <summary>
        /// ベクトル型で設定
        /// </summary>
        inline void Set(const Vector2& in_rValue) HE_NOEXCEPT
        {
            this->_fX = in_rValue._fX;
            this->_fY = in_rValue._fY;
        }

        /// <summary>
        /// ２次元補間を行う
        /// </summary>
        void SetInterporation(const Vector2& in_rBase, const Vector2& in_rULine,
                              const Vector2& in_rVLIne, const HE::Float32 in_fUScale,
                              const HE::Float32 in_fVScale) HE_NOEXCEPT;

        /// <summary>
        /// 正規化
        /// </summary>
        void Normalize() HE_NOEXCEPT;

        // 演算定義
        // 足し算
        void operator+=(const Vector2& a) HE_NOEXCEPT
        {
            this->_fX += a._fX;
            this->_fY += a._fY;
        }

        // 引き算
        void operator-=(const Vector2& a) HE_NOEXCEPT
        {
            this->_fX -= a._fX;
            this->_fY -= a._fY;
        }

        // 掛け算
        void operator*=(const Vector2& a) HE_NOEXCEPT
        {
            this->_fX *= a._fX;
            this->_fY *= a._fY;
        }

        /// <summary>
        /// Operator*s the specified scale.
        /// </summary>
        const Vector2 operator*(const HE::Float32 in_fScale) HE_NOEXCEPT
        {
            return Vector2(this->_fX * in_fScale, this->_fY * in_fScale);
        }

        // 割り算
        void operator/=(const Vector2& a) HE_NOEXCEPT
        {
            if ((this->_fX != 0.f) && (a._fX != 0.f))
            {
                this->_fX /= a._fX;
            }

            if ((this->_fY != 0.f) && (a._fY != 0.f))
            {
                this->_fY /= a._fY;
            }
        }

        // static メンバー関数で補助として利用

        /// <summary>
        /// Subs the vector2.
        /// </summary>
        static inline const Vector2 Sub(const Vector2& in_rLeft,
                                        const Vector2& in_rRight) HE_NOEXCEPT
        {
            return Vector2(in_rLeft._fX - in_rRight._fX, in_rLeft._fY - in_rRight._fY);
        }

        // ベクトルの大きさを2乗したのを取得
        // こちらの方が計算が早い
        // 比較などで利用できる
        static inline HE::Float32 LengthSquared(const Vector2& in_rVec) HE_NOEXCEPT
        {
            return (in_rVec._fX * in_rVec._fX + in_rVec._fY * in_rVec._fY);
        }

        /// <summary>
        /// ベクトル同士の距離を2乗で取得(距離の比較のみでは計算の早いこちらを利用).
        /// </summary>
        /// <param name="in_v">The in v.</param>
        /// <param name="in_v2">The in v2.</param>
        /// <returns></returns>
        static inline HE::Float32 DistanceSquared(const Vector2& in_rRight,
                                                  const Vector2& in_rLeft) HE_NOEXCEPT
        {
            Vector2 size;
            size.SetSub(in_rLeft, in_rRight);

            return LengthSquared(size);
        }

        //	ベクトルの大きさ取得
        static inline HE::Float32 Length(const Vector2& in_v) { return sqrtf(LengthSquared(in_v)); }

        //	2つのベクトルの距離取得
        static inline HE::Float32 Distance(const Vector2& in_rRight,
                                           const Vector2& in_rLeft) HE_NOEXCEPT
        {
            Vector2 size;
            size.SetSub(in_rLeft, in_rRight);

            return Length(size);
        }

        /// <summary>
        /// Dots the specified in v.
        /// </summary>
        static inline HE::Float32 Dot(const Vector2& in_rLeft, const Vector2& in_rRight) HE_NOEXCEPT
        {
            return (in_rLeft._fX * in_rRight._fX) + (in_rLeft._fY * in_rRight._fY);
        }

        /// <summary>
        /// Outputs the refection.
        /// </summary>
        static inline void OutputRefection(Vector2* out, const Vector2& in_nv,
                                           const Vector2& in_v2) HE_NOEXCEPT
        {
            Vector2 flipV(-in_v2._fX, -in_v2._fY);
            const HE::Float32 cfDot = 2 * Dot(in_nv, flipV);
            Vector2 v(in_nv._fX * cfDot, in_nv._fY * cfDot);
            v -= flipV;
            out->Set(v);
        }

        // 頻繁にアクセスする変数にはprivate指定にはしない
        HE::Float32 _fX = 0.0f, _fY = 0.0f;

    public:
        static const Vector2 Zero;
    };
}  // namespace Core::Math
