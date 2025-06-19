#pragma once

#include "Engine/Core.h"
#include "Vector2.h"

namespace Core::Math
{
    /// <summary>
    /// ベクトルクラス.
    /// このクラスはメモリを直接参照するので仮想関数を作ってはだめ.
    /// </summary>
    class Vector3 final
    {
    public:
        explicit Vector3() : _fX(0.0f), _fY(0.0f), _fZ(0.0f) {}
        explicit Vector3(HE::Float32 in_fX, HE::Float32 in_fY, HE::Float32 in_fZ)
            : _fX(in_fX), _fY(in_fY), _fZ(in_fZ)
        {
        }

        // 引数型異なるので=で代入は不可能にした
        explicit Vector3(const Vector2& in_rVec2) : _fX(in_rVec2._fX), _fY(in_rVec2._fY), _fZ(0.0f)
        {
        }

        // = で代入できるようにする
        Vector3(Vector3& in_rVec3) : _fX(in_rVec3._fX), _fY(in_rVec3._fY), _fZ(in_rVec3._fZ) {}

        // = で代入できるようにする
        Vector3(const Vector3& in_rVec3) : _fX(in_rVec3._fX), _fY(in_rVec3._fY), _fZ(in_rVec3._fZ)
        {
        }

        // 整数設定
        explicit Vector3(HE::Uint32 in_iX, HE::Uint32 in_iY, HE::Uint32 in_iZ);

        /// <summary>
        /// 関数内のローカル変数の所有権の移動に=を使うのでexplicitを使わない
        /// </summary>
        Vector3(Vector3&& other) HE_NOEXCEPT : _fX(std::move(other._fX)),
                                               _fY(std::move(other._fY)),
                                               _fZ(std::move(other._fZ))
        {
        }

        // 情報をクリア
        inline void Clear() { this->_fX = this->_fY = this->_fZ = 0.0f; }

        // 値設定
        void Set(const HE::Float32 in_fX, const HE::Float32 in_fY, const HE::Float32 in_fZ);
        void Set(const HE::Float32 in_val);

        // 掛け算
        void Mul(const HE::Float32 b);

        // 積和
        void Madd(const Vector3& a, const HE::Float32 b);

        // 積和設定
        void SetMadd(const Vector3& a, const HE::Float32 b, const Vector3& c);

        // 加算
        void SetAdd(const Vector3& a, const Vector3& b);

        // 減算
        void SetSub(const Vector3& a, const Vector3& b);

        // 積算
        void SetMul(const Vector3& a, const Vector3& b);

        // ２次元補間を行う
        void SetInterporation(const Vector3& a, const Vector3& ab, const Vector3& ac, HE::Float32 u,
                              HE::Float32 v);

        //	正規化
        void Normalize();

        // 演算定義
        void operator+=(const Vector3& a);  // 足し算
        void operator-=(const Vector3& a);  // 引き算
        void operator=(const Vector3& a);   // コピー
        void operator*=(const Vector3& a);  // 掛け算
        void operator/=(const Vector3& a);  // 割り算

        /// <summary>
        /// Operator*s the specified scale.
        /// オペレーターで戻り値はconstにしない方が良い
        /// 戻り値はどのようにアクセスするのか予測が出来ない
        /// constを入れるのか入れないのか、汎用として扱えるようにするためには制約は付けてはいけない
        /// </summary>
        /// <param name="scale">The scale.</param>
        /// <returns></returns>
        Vector3 operator*(const HE::Float32 scale) HE_NOEXCEPT
        {
            return Vector3(this->_fX * scale, this->_fY * scale, this->_fZ * scale);
        }

        /// <summary>
        /// Operator+s the specified in a.
        /// </summary>
        /// <param name="in_a">The in a.</param>
        /// <returns></returns>
        Vector3 operator+(const Vector3 in_a) HE_NOEXCEPT
        {
            return Vector3(this->_fX + in_a._fX, this->_fY + in_a._fY, this->_fZ + in_a._fZ);
        }

        /// <summary>
        /// ベクトル要素をfloatポインタで返す.
        /// </summary>
        const HE::Float32* GetAsFloatPtr() const HE_NOEXCEPT { return &this->_fX; }

        // 内積
        static HE::Float32 Dot(const Vector3& in_rV, const Vector3& in_rV2);

        // 外積
        static Vector3 Cross(const Vector3& in_rV, const Vector3& in_rV2);

        // ベクトルの大きさを2乗したのを取得
        // こちらの方が計算が早い
        // 比較などで利用できる
        static HE::Float32 LengthSquared(const Vector3&);

        // 大きさ取得
        static HE::Float32 Mag(const Vector3&);

        // 距離取得
        static HE::Float32 Distance(const Vector3& in_v, const Vector3& in_v2);

    public:
        // 頻繁にアクセスする変数にはprivate指定にはしない
        HE::Float32 _fX = 0.0f, _fY = 0.0f, _fZ = 0.0f;

        // 各要素を1にしてそれ以外の要素を0にしたベクトル
        static const Vector3 UnitX;
        static const Vector3 UnitY;
        static const Vector3 UnitZ;
        static const Vector3 Zero;
        static const Vector3 One;
    };
}  // namespace Core::Math
