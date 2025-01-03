#pragma once

#include "Engine/Common/CustomArray.h"
#include "Engine/Common/Function.h"
#include "Engine/Math/Math.h"
#include "Engine/MiniEngine.h"

namespace Render::Prticle
{
    /// <summary>
    /// 粒子データ
    /// </summary>
    struct Point
    {
        // 位置
        Core::Math::Vector3 position;

        Core::Math::Color _color;
    };

    /// <summary>
    /// 粒子群
    /// </summary>
    class Blob
    {
        HE_CLASS_COPY_NG(Blob);

    public:
        using CreateFunctionType = Core::Common::FunctionObject<Core::Common::Handle, HE::Uint32>;
        using DeleteFunctionType = Core::Common::FunctionObject<void, Core::Common::Handle>;
        using PositionFunctionType =
            Core::Common::FunctionObject<void, const Core::Common::Handle,
                                         const Core::Common::ArrayBase<Core::Math::Vector3>&>;
        using VelocityFunctionType =
            Core::Common::FunctionObject<void, const Core::Common::Handle,
                                         const Core::Common::ArrayBase<Core::Math::Vector3>&>;

    public:
        // コンストラクタ
        Blob(Core::Common::Handle, Core::Memory::UniquePtr<CreateFunctionType>,
             Core::Memory::UniquePtr<DeleteFunctionType>,
             Core::Memory::UniquePtr<PositionFunctionType>,
             Core::Memory::UniquePtr<VelocityFunctionType>);

        // デストラクタ
        virtual ~Blob();

        /// <summary>
        /// 初期化
        /// </summary>
        void Init(const HE::Uint32 in_uCount);

        /// <summary>
        /// 解放
        /// </summary>
        void Release();

        // TODO: 座標や速度,色設定
        // 座標系は正規化デバイス座標系で-1 から 1になる
        void SetPositions(const Core::Common::ArrayBase<Core::Math::Vector3>&);
        void SetVelocitys(const Core::Common::ArrayBase<Core::Math::Vector3>&);
        void SetColors(const Core::Common::ArrayBase<Core::Math::Color>&);

        Core::Common::Handle GetRenderHandle() const { return this->_renderHandle; }
        Core::Common::Handle GetDrawHandle() const { return this->_drawHandle; }

    private:
        HE::Uint32 _uCount = 0;
        Core::Common::Handle _renderHandle;
        Core::Common::Handle _drawHandle;
        Core::Memory::UniquePtr<CreateFunctionType> _upCreateFunc;
        Core::Memory::UniquePtr<DeleteFunctionType> _upDeleteFunc;
        Core::Memory::UniquePtr<PositionFunctionType> _upPositionFunc;
        Core::Memory::UniquePtr<VelocityFunctionType> _upVelocityFunc;
    };
}  // namespace Render::Prticle
