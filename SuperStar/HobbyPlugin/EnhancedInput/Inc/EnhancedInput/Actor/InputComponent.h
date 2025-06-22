#pragma once

#include "Engine/Common/CustomList.h"
// モジュール内のファイル
#include "EnhancedInput/Common.h"

// 利用モジュール一覧
#include "ActorModule.h"

namespace EnhancedInput
{
    // 前方宣言
    class InputComponent;

    /// <summary>
    /// 入力コンポーネントのストラテジークラス
    /// </summary>
    class InputStrategyBase
    {
    public:
        virtual ~InputStrategyBase() {}

        virtual HE::Bool VBegin() { return TRUE; }
        virtual HE::Bool VEnd() { return TRUE; }

        virtual void VProcessInput(const InputMap&, Actor::Object*) = 0;
    };

    /// <summary>
    /// 入力の基礎コンポーネント
    /// 機能拡張は継承したストラテジークラスで行う
    /// </summary>
    class InputComponent final : public Actor::Component,
                                 public Core::Common::LinkedListNode<InputComponent>
    {
        HE_CLASS_COPY_NG(InputComponent);
        HE_GENERATED_CLASS_BODY_HEADER(InputComponent, Component);

    public:
        InputComponent(Core::Memory::SharedPtr<InputStrategyBase>);
        virtual ~InputComponent();

        /// <summary>
        /// タスク終了
        /// </summary>
        HE::Bool VEnd() override final;

        /// <summary>
        /// 入力処理
        /// </summary>
        void ProcessInput(const InputMap&);

    private:
        Core::Memory::SharedPtr<InputStrategyBase> _spStrategy = NULL;
    };
}  // namespace EnhancedInput
