#pragma once

#include "Engine/Core.h"
#include "Matrix3.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Core::Math
{
    /// <summary>
    /// 4x4行列のクラス
    /// 行ベクトル前提で各計算処理をしている
    /// 列ベクトルにするで扱いたい場合は転置する必要がある
    /// クラスだが扱いは構造体とするので仮想関数の定義はしてはいけない
    /// </summary>
    class Matrix4 final
    {
        HE_CLASS_MOVE_NG(Matrix4);
        HE_CLASS_COPY_NG(Matrix4);

    public:
        explicit Matrix4()
        {
            // 単位行列の値
            constexpr HE::Float32 faIdent[4][4] = {
                {1.0f, 0.0f, 0.0f, 0.0f},
                {0.0f, 1.0f, 0.0f, 0.0f},
                {0.0f, 0.0f, 1.0f, 0.0f},
                {0.0f, 0.0f, 0.0f, 1.0f},
            };
            this->Set(faIdent);
        }

        inline void Set(Matrix4& in_rMat)
        {
            ::memcpy(this->_faMat, in_rMat._faMat, 4 * 4 * sizeof(HE::Float32));
        }

        inline void Set(const HE::Float32 in_faaMat[4][4])
        {
            ::memcpy(this->_faMat, in_faaMat, 4 * 4 * sizeof(HE::Float32));
        }

        // Floatのポインタとして取得
        const HE::Float32* GetAsFloatPtr() const
        {
            return reinterpret_cast<const HE::Float32*>(&this->_faMat[0][0]);
        }

        void Mul(const Matrix4& in_rSrcMat)
        {
            // 行列同士の計算で行側と列側でそれぞれVector4クラスに格納
            // Vector4クラス同士の掛け算で行列の各行を作成
            auto row01 = Vector4(this->_faMat[0][0], this->_faMat[0][1], this->_faMat[0][2],
                                 this->_faMat[0][3]);
            auto row02 = Vector4(this->_faMat[1][0], this->_faMat[1][1], this->_faMat[1][2],
                                 this->_faMat[1][3]);
            auto row03 = Vector4(this->_faMat[2][0], this->_faMat[2][1], this->_faMat[2][2],
                                 this->_faMat[2][3]);
            auto row04 = Vector4(this->_faMat[3][0], this->_faMat[3][1], this->_faMat[3][2],
                                 this->_faMat[3][3]);

            auto col01 = Vector4(in_rSrcMat._faMat[0][0], in_rSrcMat._faMat[1][0],
                                 in_rSrcMat._faMat[2][0], in_rSrcMat._faMat[3][0]);
            auto col02 = Vector4(in_rSrcMat._faMat[0][1], in_rSrcMat._faMat[1][1],
                                 in_rSrcMat._faMat[2][1], in_rSrcMat._faMat[3][1]);
            auto col03 = Vector4(in_rSrcMat._faMat[0][2], in_rSrcMat._faMat[1][2],
                                 in_rSrcMat._faMat[2][2], in_rSrcMat._faMat[3][2]);
            auto col04 = Vector4(in_rSrcMat._faMat[0][3], in_rSrcMat._faMat[1][3],
                                 in_rSrcMat._faMat[2][3], in_rSrcMat._faMat[3][3]);

            auto newRow01 = Vector4(row01.AddMul(col01), row01.AddMul(col02), row01.AddMul(col03),
                                    row01.AddMul(col04));
            auto newRow02 = Vector4(row02.AddMul(col01), row02.AddMul(col02), row02.AddMul(col03),
                                    row02.AddMul(col04));
            auto newRow03 = Vector4(row03.AddMul(col01), row03.AddMul(col02), row03.AddMul(col03),
                                    row03.AddMul(col04));
            auto newRow04 = Vector4(row04.AddMul(col01), row04.AddMul(col02), row04.AddMul(col03),
                                    row04.AddMul(col04));

            HE::Float32 temp[4][4] = {
                {newRow01._fX, newRow01._fY, newRow01._fZ, newRow01._fW},
                {newRow02._fX, newRow02._fY, newRow02._fZ, newRow02._fW},
                {newRow03._fX, newRow03._fY, newRow03._fZ, newRow03._fW},
                {newRow04._fX, newRow04._fY, newRow04._fZ, newRow04._fW},
            };

            ::memcpy(this->_faMat, temp, sizeof(HE::Float32) * 4 * 4);
        }

        // 平行移動ベクトル取得
        void OutputTranslation(Vector3* out);

        /// <summary>
        /// 逆行列に変換したのを出力
        /// </summary>
        void OutputInvert(Matrix4* out);

        /// <summary>
        /// 行列式を取得.
        /// </summary>
        HE::Float32 GetDet();

        /// <summary>
        /// 余因子行列を出力.
        /// </summary>
        void OutputYoinshi(Matrix4* out);

        /// <summary>
        /// 行と列を指定した3x3の余因子行列を出力.
        /// </summary>
        void OutputYoinshi3x3(Matrix3* out_pMat, const HE::Uint32 in_iRow,
                              const HE::Uint32 in_iCol);

        /// <summary>
        /// 転置行列に変換.
        /// </summary>
        void ChangeTentsu();

        /// <summary>
        /// 各要素にスケール値を掛ける.
        /// </summary>
        void Scale(const HE::Float32);

        /// <summary>
        /// スケール設定した行列出力
        /// </summary>
        static void OutputScale(Matrix4* out, const HE::Float32 in_fX, const HE::Float32 in_fY,
                                const HE::Float32 in_fZ)
        {
            const HE::Float32 faTemp[4][4] = {
                {in_fX, 0.0f, 0.0f, 0.0f},
                {0.0f, in_fY, 0.0f, 0.0f},
                {0.0f, 0.0f, in_fZ, 0.0f},
                {0.0f, 0.0f, 0.0f, 1.0f},
            };
            out->Set(faTemp);
        }

        /// <summary>
        /// Creates the scale.
        /// </summary>
        static void OutputScale(Matrix4* out, const Math::Vector3& in_rScale)
        {
            OutputScale(out, in_rScale._fX, in_rScale._fY, in_rScale._fZ);
        }

        /// <summary>
        /// Creates the scale.
        /// </summary>
        static void OutputScale(Matrix4* out, const HE::Float32 in_fScale)
        {
            OutputScale(out, in_fScale, in_fScale, in_fScale);
        }

        /// <summary>
        /// Creates the rotation z.
        /// 回転する単位はラジアンなので注意です。
        /// 1ラジアン = 2pi / 360
        /// なぜ?
        ///   2pi = 360度なのでこれを360分割すれば1ラジアンになるから
        /// </summary>
        static void OutputRotationZ(Matrix4* out, const HE::Float32 in_fRadian);

        /// <summary>
        /// Creates the translation.
        /// </summary>
        static void OutputTranslation(Matrix4* out, const Vector3& in_rPos);

        // 縦横のサイズを元に単位正方形の座標系に行列変換する行列作成
        static void OutputSimpleViewProj(Matrix4* out, const HE::Float32 in_fWidth,
                                         const HE::Float32 in_fHeight);

        // カメラ情報からビュー行列生成
        static void OutputLookAt(Matrix4* out, const Vector3& in_rEye, const Vector3& in_rTarget,
                                 const Vector3& in_rUp);

        // 正射影行列作成
        static void OutputOrtho(Matrix4* out, const HE::Float32 in_fWidth,
                                const HE::Float32 in_fHeight, const HE::Float32 in_fNear,
                                const HE::Float32 in_fFar);

        // 透視射影行列作成
        static void OutputPerspectiveFOV(Matrix4* out, const HE::Float32 in_fFov,
                                         const HE::Float32 in_fWidth, const HE::Float32 in_fHeight,
                                         const HE::Float32 in_fNear, const HE::Float32 in_fFar);

    public:
        // 単位行列定義
        static const Matrix4 Identity;

        HE::Float32 _faMat[4][4];
    };
}  // namespace Core::Math
