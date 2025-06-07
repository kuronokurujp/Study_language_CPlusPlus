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

        auto pInputMap = &in_mInputMap;
        HE_ASSERT(pInputMap);

        // TODO: 入力マップとの関連付けが必要
        // UIのユーザー入力があるかチェック
        EnhancedInput::InputDataVector* pInputDataVector = NULL;
        {
            for (auto i = 0; i < this->_vActiveInput.Capacity(); ++i)
            {
                if (pInputMap->Contains(this->_vActiveInput[i]))
                {
                    pInputDataVector = &pInputMap->FindKey(this->_vActiveInput[i])->_data;
                    break;
                }
            }
        }

        if (pInputDataVector == NULL)
        {
            // UIのユーザー入力がない場合は何もしない
            return;
        }

        // 入力結果をWidgetに通知
        Widget* pWidget = reinterpret_cast<Widget*>(in_pSelfObject);
        HE_ASSERT(pWidget != NULL);

        // Widgetアクターに設定しているUIWidgetコンポーネントを全て取得
        Core::Common::FixedStack<Actor::Component*, 128> sWidgetComponent;
        pWidget->OutputChildrenComponent(&sWidgetComponent, &UIWidgetComponent::CLASS_RTTI);

        HE::Uint32 _uSize = sWidgetComponent.Size();
        Core::Common::Handle handle;

        // UIに関わる, マウスのクリックやキーボードなどの結果を受け取り, 各入力端末へ通知する
        // ルーター側でUIの入力処理を制御
        // UIレイヤーに応じてボタンを押せないとか
        for (HE::Uint32 i = 0; i < pInputDataVector->Size(); ++i)
        {
            if ((*pInputDataVector)[i].eType == EnhancedInput::EInputType::EInputType_Touch)
            {
                while (sWidgetComponent.Empty() == FALSE)
                {
                    UIWidgetComponent* c =
                        reinterpret_cast<UIWidgetComponent*>(sWidgetComponent.PopBack());
                    c->VOnTouch((*pInputDataVector)[i].item.touch);
                }
            }
        }
    }
}  // namespace UI
