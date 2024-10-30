#pragma once

#include "Engine/MiniEngine.h"

// 利用モジュール
#include "ActorModule.h"

namespace InGame
{
    /// <summary>
    /// レンダリング基本コンポーネント
    /// </summary>
    class InGameRendererBaseComponent : public Actor::Component
    {
        HE_CLASS_COPY_NG(InGameRendererBaseComponent);
        HE_CLASS_MOVE_NG(InGameRendererBaseComponent);
        HE_GENERATED_CLASS_BODY_HEADER(InGameRendererBaseComponent, Actor::Component);

    public:
        InGameRendererBaseComponent() = default;

        void SetViewHandle(Core::Common::Handle&);
        void SetTransformHandle(Core::Common::Handle&);

        /// <summary>
        /// タスク開始
        /// </summary>
        Bool VBegin() override;

        /// <summary>
        /// コンポーネントの更新
        /// </summary>
        void VUpdate(const Float32 in_fDt) override final;

    protected:
        virtual void _VRenderer(Core::Common::Handle& in_rViewHandle,
                                class Actor::TransformComponent*) = 0;

    private:
        Core::Common::Handle _viewHandle;
        Core::Common::Handle _transformHandle;
    };

}  // namespace InGame
