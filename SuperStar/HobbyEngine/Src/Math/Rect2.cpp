#include "Engine/Math/Rect2.h"

namespace Core::Math::RC
{
    Rect2D Rect2D::ConvertFromAnchor(const Vector2& in_rPos, const Vector2& in_rSize,
                                     const EAnchor in_eAnchor)
    {
        HE::Float32 _fX = 0.0f, _fY = 0.0f;

        switch (in_eAnchor)
        {
            // パラメータは左上を基準にしているので
            case EAnchor_Left:
            {
                _fX = in_rPos._fX + (in_rSize._fX * 0.5f);
                _fY = in_rPos._fY - (in_rSize._fY * 0.5f);
                break;
            }

            case EAnchor_Center:
            {
                _fX = in_rPos._fX;
                _fY = in_rPos._fY;
                break;
            }
        }

        return Rect2D(_fX, _fY, in_rSize._fX, in_rSize._fY);
    }

    Rect2D::Rect2D(const HE::Float32 in_fX, const HE::Float32 in_fY, const HE::Float32 in_fW,
                   const HE::Float32 in_fH)
    {
        this->Set(in_fX, in_fY, in_fW, in_fH);
    }

    void Rect2D::Set(const HE::Float32 in_fX, const HE::Float32 in_fY, const HE::Float32 in_fW,
                     const HE::Float32 in_fH)
    {
        this->Clear();

        this->_fX = in_fX;
        this->_fY = in_fY;
        this->_fW = in_fW;
        this->_fH = in_fH;
    }

    /// <summary>
    /// Gets the position.
    /// </summary>
    const Vector2 Rect2D::GetPos() const
    {
        HE::Float32 _fX = 0.0f, _fY = 0.0f;

        _fX = this->_fX;
        _fY = this->_fY;
        return (Vector2(_fX, _fY));
    }

    /// <summary>
    /// 矩形同士が交差しているかどうか
    /// </summary>
    HE::Bool Rect2D::IsRectIntersect(const Rect2D& in_rOrderRect) const
    {
        const Vector2& rPos  = this->GetPos();
        const Vector2& rLine = Vector2::Sub(rPos, in_rOrderRect.GetPos());

        auto fSelfW = this->WidthHalf();
        auto fSelfH = this->HeightHalf();

        HE::Float32 fOrderW = 0.0f, fOrderH = 0.0f;
        fOrderW = in_rOrderRect.WidthHalf();
        fOrderH = in_rOrderRect.HeightHalf();

        const HE::Float32 fW = fSelfW + fOrderW;
        const HE::Float32 fH = fSelfH + fOrderH;

        // 矩形同士の距離が矩形同士を足した縦横サイズがより小さい場合は矩形が重なっている
        if (fW < fabs(rLine._fX)) return FALSE;
        if (fH < fabs(rLine._fY)) return FALSE;

        return TRUE;
    }

    /// <summary>
    /// 座標が矩形の中に入っているか
    /// </summary>
    HE::Bool Rect2D::IsPointInside(const Vector2& in_rPos) const
    {
        // TODO: 左右上下の矩形情報を計算
        HE::Float32 fRight  = this->_fX + this->WidthHalf();
        HE::Float32 fLeft   = this->_fX - this->WidthHalf();
        HE::Float32 fTop    = this->_fY + this->HeightHalf();
        HE::Float32 fBottom = this->_fY - this->HeightHalf();

        // 左右の内にあるか
        if (fRight < in_rPos._fX) return FALSE;
        if (in_rPos._fX < fLeft) return FALSE;
        // 上下の内にあるか
        if (fTop < in_rPos._fY) return FALSE;
        if (in_rPos._fY < fBottom) return FALSE;

        return TRUE;
    }
}  // namespace Core::Math::RC
