#include "UIWidget.h"

namespace UI
{
    void UIWidgetComponent::VSetup(const HE::Bool in_bAutoDelete)
    {
        Actor::Component::VSetup(in_bAutoDelete);
        this->_Clear();
    }

    HE::Bool UIWidgetComponent::VBegin()
    {
        if (Component::VBegin() == FALSE) return FALSE;

        HE_ASSERT(this->_viewHandle.Null() == FALSE);
        //        if (this->_viewHandle.Null()) return FALSE;

        this->_transformHandle = this->_pOwner->AddComponent<Actor::TransformComponent>(
            0, Actor::Component::EPriorty::EPriorty_Main);
        if (this->_transformHandle.Null()) return FALSE;

        this->GetTransformComponent()->SetPos(Core::Math::Vector3(this->_pos));

        return TRUE;
    }

    HE::Bool UIWidgetComponent::VEnd()
    {
        if (Component::VEnd() == FALSE) return FALSE;
        if (this->_pOwner->RemoveComponent(&this->_transformHandle) == FALSE)
        {
        }

        this->_Clear();

        return TRUE;
    }

    void UIWidgetComponent::SetPos(const Core::Math::Vector2& in_rPos)
    {
        this->_pos = in_rPos;

        if (this->_transformHandle.Null()) return;

        auto pTrans = this->GetTransformComponent();
        if (pTrans)
        {
            pTrans->SetPos(Core::Math::Vector3(this->_pos));
        }
    }

    void UIWidgetComponent::SetSize(const Core::Math::Vector2& in_rSize)
    {
        this->_size = in_rSize;
    }

    Actor::TransformComponent* UIWidgetComponent::GetTransformComponent()
    {
        return this->_pOwner->GetComponent<Actor::TransformComponent>(this->_transformHandle);
    }

}  // namespace UI
