#pragma once

#include "Engine/MiniEngine.h"
#include "InputComponent.h"

// 利用モジュール一覧
#include "ActorModule.h"

// モジュール内のファイル
#include "EnhancedInput/Common.h"

namespace EnhancedInput
{
    // 前方宣言
    class InputComponent;

    /// <summary>
    ///  入力システムに対応したアクター管理のデコレーター
    /// アクター管理に入力システムを適応するにはこれを使う
    /// </summary>
    class ActorManagerDecoraterWithInputSystem final : public Actor::ActorManagerDecoraterlnterface
    {
    public:
        HE::Bool VStart(Actor::ActorManager*) override final;

        /// <summary>
        /// 管理下にあるアクターに入力状態を送信
        /// </summary>
        void ProcessInput(const InputMap&);

        /// <summary>
        /// 入力コンポーネントの登録・解除
        /// </summary>
        void VOnActorRegistComponent(Actor::Component* in_pComponent) override final;
        void VOnActorUnRegistComponent(Actor::Component* in_pComponent) override final;

    private:
        Core::Common::CustomList<EnhancedInput::InputComponent> _lstInputComponent;
    };
}  // namespace EnhancedInput
