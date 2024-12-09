#include "InGameRendererBaseComponent.h"

// 利用するモジュール一覧
#include "RenderModule.h"

namespace InGame
{
    void InGameRendererBaseComponent::SetViewHandle(const Core::Common::Handle& in_rHandle)
    {
        this->_viewHandle = in_rHandle;
    }

    void InGameRendererBaseComponent::SetTransformHandle(const Core::Common::Handle& in_rHandle)
    {
        this->_transformHandle = in_rHandle;
    }

    HE::Bool InGameRendererBaseComponent::VBegin()
    {
        if (Actor::Component::VBegin() == FALSE) return FALSE;

        // 座標情報を持つコンポーネントのハンドルを取得
        // このコンポーネントがないと処理できない
        auto [h, c] =
            this->Owner()->GetComponentHandleAndComponent(&Actor::TransformComponent::CLASS_RTTI);
        HE_ASSERT(h.Null() == FALSE);
        this->_transformHandle = h;

        return TRUE;
    }

    void InGameRendererBaseComponent::VUpdate(const HE::Float32 in_fDt)
    {
        Actor::Component::VUpdate(in_fDt);

        auto pTrans =
            this->Owner()->GetComponent<Actor::TransformComponent>(this->_transformHandle);
        HE_ASSERT(pTrans);
        this->_VRenderer(this->_viewHandle, pTrans);
    }

}  // namespace InGame
