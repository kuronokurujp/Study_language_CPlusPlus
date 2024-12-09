#pragma once

#include "Engine/Common/CustomStack.h"
#include "InGame/Shot/InGameShotStrategyInterface.h"

// エンジン最小インクルード
#include "Engine/MiniEngine.h"

// 利用モジュール
#include "ActorModule.h"

namespace InGame
{
    /// <summary>
    /// 弾を打つコンポーネント
    /// </summary>
    class InGameShotComponent final : public Actor::Component
    {
        HE_CLASS_COPY_NG(InGameShotComponent);
        HE_CLASS_MOVE_NG(InGameShotComponent);
        HE_GENERATED_CLASS_BODY_HEADER(InGameShotComponent, Actor::Component);

    public:
        InGameShotComponent() : Actor::Component() {}
        virtual ~InGameShotComponent() = default;

        /// <summary>
        /// 終了
        /// </summary>
        HE::Bool VEnd() override final;

        /// <summary>
        /// コンポーネントの更新
        /// </summary>
        void VUpdate(const HE::Float32 in_fDt) override final;

        /// <summary>
        /// 弾を打つ機能設定
        /// </summary>
        void SetStrategy(Core::Memory::SharedPtr<InGameShotStrategyInterface> in_spStrategy)
        {
            this->_spStrategy = in_spStrategy;
        }

        /// <summary>
        /// 弾を打つ
        /// </summary>
        void Shot(const Core::Math::Vector2& in_rPos, const Core::Math::Vector2& in_rDir,
                  const HE::Uint32 in_uCollisionHashCode);

    private:
        Core::Memory::SharedPtr<InGameShotStrategyInterface> _spStrategy;
        Core::Common::FixedStack<InGameShotStrategyInterface::ShotConfig, 256> _sCatchShot;
    };

}  // namespace InGame
