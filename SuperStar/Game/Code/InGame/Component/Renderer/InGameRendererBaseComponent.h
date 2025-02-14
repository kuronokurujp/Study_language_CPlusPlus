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

        void SetViewHandle(const Core::Common::Handle&);
        void SetTransformHandle(const Core::Common::Handle&);

        /// <summary>
        /// タスク開始
        /// </summary>
        HE::Bool VBegin() override;

        /// <summary>
        /// コンポーネントの更新
        /// </summary>
        void VUpdate(const HE::Float32 in_fDt) override final;

        virtual void VSetSize(const Core::Math::Vector2&)   = 0;
        virtual const Core::Math::Vector2& VGetSize() const = 0;

        /// <summary>
        /// TODO: 画面内かどうか
        /// </summary>
        /// <returns></returns>
        HE::Bool IsScreenIn();

    protected:
        virtual void _VRenderer(const Core::Common::Handle& in_rViewHandle,
                                class Actor::TransformComponent*) = 0;

    private:
        Core::Common::Handle _viewHandle;
        Core::Common::Handle _transformHandle;
    };

}  // namespace InGame
