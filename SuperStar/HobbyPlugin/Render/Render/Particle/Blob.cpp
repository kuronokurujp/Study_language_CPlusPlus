#include "Blob.h"

#include "./../Command/Command.h"

namespace Render::Prticle
{
    Blob::Blob(Core::Common::Handle in_renderHandle,
               Core::Memory::UniquePtr<CreateFunctionType> in_upCreateFunc,
               Core::Memory::UniquePtr<DeleteFunctionType> in_upDeleteFunc,
               Core::Memory::UniquePtr<PositionFunctionType> in_upPositionFunc,
               Core::Memory::UniquePtr<VelocityFunctionType> in_upVelocityFunc)
    {
        this->_renderHandle   = in_renderHandle;
        this->_drawHandle     = NullHandle;
        this->_uCount         = 0;
        this->_upCreateFunc   = std::move(in_upCreateFunc);
        this->_upDeleteFunc   = std::move(in_upDeleteFunc);
        this->_upPositionFunc = std::move(in_upPositionFunc);
        this->_upVelocityFunc = std::move(in_upVelocityFunc);
    }

    Blob::~Blob()
    {
        this->Release();
    }

    void Blob::Init(const HE::Uint32 in_uCount)
    {
        this->_uCount = in_uCount;

        // プラットフォームからパーティクルのハンドルを生成
        this->_drawHandle = this->_upCreateFunc->CallByRef(this->_uCount);
        HE_ASSERT(this->_drawHandle.Null() == FALSE);
    }

    void Blob::Release()
    {
        // パーティクルハンドル解放
        this->_upDeleteFunc->CallByRef(this->_drawHandle);

        HE_SAFE_DELETE_UNIQUE_PTR(this->_upCreateFunc);
        HE_SAFE_DELETE_UNIQUE_PTR(this->_upDeleteFunc);
        HE_SAFE_DELETE_UNIQUE_PTR(this->_upPositionFunc);
        HE_SAFE_DELETE_UNIQUE_PTR(this->_upVelocityFunc);
    }

    void Blob::SetPositions(const Core::Common::ArrayBase<Core::Math::Vector3>& in_rArray)
    {
        // パーティクルの座標を書き込む
        this->_upPositionFunc->CallByRef(this->_drawHandle, in_rArray);
    }

    void Blob::SetVelocitys(const Core::Common::ArrayBase<Core::Math::Vector3>& in_rArray)
    {
        // パーティクルの座標を書き込む
        this->_upVelocityFunc->CallByRef(this->_drawHandle, in_rArray);
    }

    void Blob::SetColors(const Core::Common::ArrayBase<Core::Math::Color>& in_rArray)
    {
        // TODO: パーティクルの色を書き込む
    }

}  // namespace Render::Prticle
