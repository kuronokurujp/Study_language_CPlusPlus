#pragma once

#include "EnhancedInput/Actor/InputComponent.h"

namespace UI
{
    /// <summary>
    /// UIの入力ルーター
    /// UIのWidgetのタップやボタン入力を行い,
    /// 各Widgetへ結果を送信する
    /// </summary>
    class UIInputRouterStrategy final : public EnhancedInput::InputStrategyBase
    {
        HE_CLASS_COPY_NG(UIInputRouterStrategy);

    public:
        // TODO: インプットマップの設定
        UIInputRouterStrategy() : EnhancedInput::InputStrategyBase() {}

        void VProcessInput(const EnhancedInput::InputMap&, Actor::Object*) override final;
    };
}  // namespace UI
