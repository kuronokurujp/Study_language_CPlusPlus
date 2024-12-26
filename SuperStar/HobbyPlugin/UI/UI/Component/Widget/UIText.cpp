#include "UIText.h"

#include "Engine/Engine.h"

// 依存するモジュール一覧
#include "ActorModule.h"
#include "LocalizationModule.h"
#include "RenderModule.h"

namespace UI
{
    void UITextComponent::VUpdate(const HE::Float32 in_fDeltaTime)
    {
        UIWidgetComponent::VUpdate(in_fDeltaTime);

        // ローカライズテキストならローカライズテキストを取得
        if (0 < this->_szLocGroup.Size())
        {
            // TODO: 言語切り替えが必要
            auto pLocalModule = HE_ENGINE.ModuleManager().Get<Localization::LocalizationModule>();
            HE_ASSERT(pLocalModule);

            this->_szDrawText =
                pLocalModule->Text(HE_STR_TEXT("JP"), this->_szLocGroup.Str(), this->_szText.Str());
        }
        else
        {
            this->_szDrawText = this->_szText;
        }

        Core::Math::Vector2 pos;

        auto pTrans = this->GetTransformComponent();
        HE_ASSERT(pTrans);

        pTrans->TransformLocalToWorldPos2D(&pos, this->_rect.Pos());

        Render::Command2DTextDraw(this->_viewHandle, pos, this->_szDrawText, this->_uFontSize,
                                  {this->_uColor}, this->_eAnchor, 2.0f);
    }
}  // namespace UI
