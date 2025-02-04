#include "Rect2.h"

namespace Core::Math
{
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
        HE::Float32 _fX = 0.0f, _fY = 0.0f;

        switch (this->_eAnchor)
        {
            case EAnchor_Left:
            {
                _fX = this->_fLeft;
                _fY = this->_fTop;
                break;
            }

            case EAnchor_Center:
            {
                _fX = this->_fLeft + (this->Width() * 0.5f);
                _fY = this->_fTop + (this->Height() * 0.5f);

                break;
            }
        }
        return (Vector2(_fX, _fY));
    }

    /// <summary>
    /// 引数の矩形が中にあるかどうか.
    /// </summary>
    HE::Bool Rect2::InSideRect(const Rect2& in_rOrderRect) const
    {
        const Vector2& rPos  = this->Pos();
        const Vector2& rLine = Vector2::Sub(rPos, in_rOrderRect.Pos());

        HE::Float32 fSelfW = 0.0f, fSelfH = 0.0f;

        switch (this->_eAnchor)
        {
            case EAnchor::EAnchor_Center:
            {
                fSelfW = this->WidthHalf();
                fSelfH = this->HeightHalf();
                break;
            }
            case EAnchor::EAnchor_Left:
            {
                fSelfW = this->Width();
                fSelfH = this->Height();
                break;
            }
        }

        HE::Float32 fOrderW = 0.0f, fOrderH = 0.0f;
        switch (in_rOrderRect._eAnchor)
        {
            case EAnchor::EAnchor_Center:
            {
                fOrderW = in_rOrderRect.WidthHalf();
                fOrderH = in_rOrderRect.HeightHalf();
                break;
            }
            case EAnchor::EAnchor_Left:
            {
                fOrderW = in_rOrderRect.Width();
                fOrderH = in_rOrderRect.Height();
                break;
            }
        }

        const HE::Float32 fW = fSelfW + fOrderW;
        const HE::Float32 fH = fSelfH + fOrderH;

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
