#include "UIInputRouter.h"

#include "ActorModule.h"
#include "EnhancedInputModule.h"
#include "UI/Component/Widget/UIWidget.h"
#include "UI/Widget.h"

namespace UI
{
    UIInputRouterStrategy::UIInputRouterStrategy(ActiveInputVector& in_vrActiveInput)
        : EnhancedInput::InputStrategyBase()
    {
        this->_vActiveInput = in_vrActiveInput;
    }

    void UIInputRouterStrategy::VProcessInput(const EnhancedInput::InputMap& in_mInputMap,
                                              Actor::Object* in_pSelfObject)
    {
        HE_ASSERT_RETURN(in_pSelfObject);

        // 入力結果をWidgetに通知
        Widget* pWidget = reinterpret_cast<Widget*>(in_pSelfObject);
        HE_ASSERT_RETURN(pWidget != NULL);

        // Widgetアクターに設定しているUIWidgetコンポーネントを全て取得
        Core::Common::FixedStack<Actor::Component*, 128> sWidgetComponent;
        pWidget->OutputChildrenComponent(&sWidgetComponent, &UIWidgetComponent::CLASS_RTTI);

        HE::Uint32 uSize = sWidgetComponent.Size();
        HE_ASSERT_RETURN(0 < uSize);

        // UIのユーザー入力があるかチェック
        for (auto i = 0; i < this->_vActiveInput.Capacity(); ++i)
        {
            if (in_mInputMap.Contains(this->_vActiveInput[i]) == FALSE) continue;

            const auto& rInputDataVector = in_mInputMap.FindKey(this->_vActiveInput[i])->_data;

            // UIに関わる, マウスのクリックやキーボードなどの結果を受け取り,
            // 各入力端末へ通知する ルーター側でUIの入力処理を制御
            // UIレイヤーに応じてボタンを押せないとか
            for (HE::Uint32 i = 0; i < rInputDataVector.Size(); ++i)
            {
                const auto* pInputData = rInputDataVector.GetPtr(i);
                HE_ASSERT_RETURN(pInputData);

                if (pInputData->eType == EnhancedInput::EInputType::EInputType_Touch)
                {
                    while (sWidgetComponent.Empty() == FALSE)
                    {
                        auto c = reinterpret_cast<UIWidgetComponent*>(sWidgetComponent.PopBack());
                        c->VOnTouch(this->_vActiveInput[i].Str(), pInputData->item.touch);
                    }
                }
            }
        }
    }
}  // namespace UI
