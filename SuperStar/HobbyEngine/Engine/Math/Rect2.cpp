#include "Rect2.h"

namespace Core::Math
{
    /*
        // 2Dの矩形クラス
        Rect2::Rect2(const HE::Float32 in_fX, const HE::Float32 in_fY, const HE::Float32 in_fW,
                     const HE::Float32 in_fH, const EAnchor in_eAnchor)
        {
            this->SetPosition(in_fX, in_fY, in_fW, in_fH, in_eAnchor);
        }
        */

    /*
        Rect2::Rect2(const Rect2& in_rRect)
        {
            ::memcpy(this, &in_rRect, sizeof(Rect2));
        }
        */

    void Rect2::SetPosition(const HE::Float32 in_fX, const HE::Float32 in_fY,
                            const HE::Float32 in_fW, const HE::Float32 in_fH,
                            const EAnchor in_eAnchor)
    {
        this->Clear();

        switch (in_eAnchor)
        {
            case EAnchor_Left:
            {
                this->_fLeft   = in_fX;
                this->_fRight  = in_fX + in_fW;
                this->_fTop    = in_fY;
                this->_fBottom = in_fY + in_fH;

                break;
            }
            case EAnchor_Center:
            {
                HE::Float32 fHalfW = in_fW * 0.5f;
                HE::Float32 fHalfH = in_fH * 0.5f;

                this->_fLeft   = in_fX - fHalfW;
                this->_fRight  = in_fX + fHalfW;
                this->_fTop    = in_fY - fHalfH;
                this->_fBottom = in_fY + fHalfH;

                break;
            }
        }
        this->_eAnchor = in_eAnchor;
    }

    void Rect2::SetRect(const HE::Float32 in_fLeft, const HE::Float32 in_fRight,
                        const HE::Float32 in_fTop, const HE::Float32 in_fBottom,
                        const EAnchor in_eAnchor)
    {
        this->_fLeft   = in_fLeft;
        this->_fRight  = in_fRight;
        this->_fTop    = in_fTop;
        this->_fBottom = in_fBottom;
        this->_eAnchor = in_eAnchor;
    }

    /// <summary>
    /// Gets the position.
    /// </summary>
    const Vector2 Rect2::Pos() const
    {
        HE::Float32 fX = 0.0f, fY = 0.0f;

        switch (this->_eAnchor)
        {
            case EAnchor_Left:
            {
                fX = this->_fLeft;
                fY = this->_fTop;
                break;
            }

            case EAnchor_Center:
            {
                fX = this->_fLeft + (this->Width() * 0.5f);
                fY = this->_fTop + (this->Height() * 0.5f);

                break;
            }
        }
        return (Vector2(fX, fY));
    }

    /// <summary>
    /// Ins the side rect.
    /// </summary>
    HE::Bool Rect2::InSideRect(const Rect2& in_rOrderRect) const
    {
        const Vector2& rPos  = this->Pos();
        const Vector2& rLine = Vector2::Sub(rPos, in_rOrderRect.Pos());

        const HE::Float32 fW = this->WidthHalf() + in_rOrderRect.WidthHalf();
        const HE::Float32 fH = this->HeightHalf() + in_rOrderRect.HeightHalf();

        if (fW < fabs(rLine._fX)) return FALSE;
        if (fH < fabs(rLine._fY)) return FALSE;

        return TRUE;
    }

    /// <summary>
    /// 座標が矩形の中に入っているか
    /// </summary>
    HE::Bool Rect2::InSidePoint(const Vector2& in_rPos) const
    {
        if (this->_fRight < in_rPos._fX) return FALSE;

        if (in_rPos._fX < this->_fLeft) return FALSE;

        if (this->_fBottom < in_rPos._fY) return FALSE;

        if (in_rPos._fY < this->_fTop) return FALSE;

        return TRUE;
    }

    /// <summary>
    /// Sets the position.
    /// </summary>
    void Rect2::_SetPos(const HE::Float32 in_fX, const HE::Float32 in_fY)
    {
        switch (this->_eAnchor)
        {
            case EAnchor_Left:
            {
                this->_fLeft   = in_fX;
                this->_fRight  = in_fX + this->Width();
                this->_fTop    = in_fY;
                this->_fBottom = in_fY + this->Height();
                break;
            }
            case EAnchor_Center:
            {
                const HE::Float32 halfW = this->WidthHalf();
                const HE::Float32 halfH = this->HeightHalf();

                this->_fLeft   = in_fX - halfW;
                this->_fRight  = in_fX + halfW;
                this->_fTop    = in_fY - halfH;
                this->_fBottom = in_fY + halfH;

                break;
            }
        }
    }
}  // namespace Core::Math
