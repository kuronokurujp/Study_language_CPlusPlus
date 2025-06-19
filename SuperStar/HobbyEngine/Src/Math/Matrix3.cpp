#include "Engine/Math/Matrix3.h"

#include "Engine/Memory/Memory.h"

namespace Core::Math
{
    Matrix3::Matrix3()
    {
        HE_CLS_MEM(&this->_faaMat[0][0], sizeof(this->_faaMat[0][0]) * 3 * 3);
    }

    /// <summary>
    /// 行列式を取得.
    /// </summary>
    HE::Float32 Matrix3::GetDet()
    {
        const HE::Float32 a = this->_faaMat[0][0] * (this->_faaMat[1][1] * this->_faaMat[2][2] -
                                                     this->_faaMat[1][2] * this->_faaMat[2][1]);
        const HE::Float32 b = this->_faaMat[0][1] * (this->_faaMat[1][2] * this->_faaMat[2][0] -
                                                     this->_faaMat[1][0] * this->_faaMat[2][2]);
        const HE::Float32 c = this->_faaMat[0][2] * (this->_faaMat[1][0] * this->_faaMat[2][1] -
                                                     this->_faaMat[1][1] * this->_faaMat[2][0]);

        return (a + b - c);
    }
}  // namespace Core::Math
