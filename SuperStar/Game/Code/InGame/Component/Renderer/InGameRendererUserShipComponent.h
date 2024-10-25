#pragma once

#include "Engine/MiniEngine.h"

// 利用モジュール
#include "ActorModule.h"

namespace InGame
{
    /// <summary>
    /// ユーザー船のレンダリングコンポーネント
    /// </summary>
    class InGameRendererUserShipComponent final : public Actor::Component
    {
        HE_CLASS_COPY_NG(InGameRendererUserShipComponent);
        HE_CLASS_MOVE_NG(InGameRendererUserShipComponent);
        HE_GENERATED_CLASS_BODY_HEADER(InGameRendererUserShipComponent, Actor::Component);

    public:
        InGameRendererUserShipComponent() = default;

        void SetViewHandle(Core::Common::Handle&);
        void SetTransformHandle(Core::Common::Handle&);
        void SetSize(Core::Math::Vector2&);

        /// <summary>
        /// タスク開始
        /// </summary>
        Bool VBegin() override final;

        /// <summary>
        /// コンポーネントの更新
        /// </summary>
        void VUpdate(const Float32 in_fDt) override final;

    private:
        Core::Common::Handle _viewHandle;
        Core::Common::Handle _transformHandle;
        Core::Math::Vector2 _size;
    };

}  // namespace InGame
