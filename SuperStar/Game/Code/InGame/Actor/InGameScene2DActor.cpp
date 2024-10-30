#include "InGameScene2DActor.h"

#include "Actor/Component/TransformComponent.h"

namespace InGame
{
    InGameScene2DActor::InGameScene2DActor() : Actor::Object()
    {
        this->_Clear();
    }

    Bool InGameScene2DActor::VBegin()
    {
        if (Actor::Object::VBegin() == FALSE) return FALSE;

        // 座標関連のコンポーネント追加
        {
            this->_transformHandle = this->AddComponent<Actor::TransformComponent>(1);
            HE_ASSERT((this->_transformHandle.Null() == FALSE) &&
                      "トランスフォームコンポーネントの追加失敗");

            // 事前に設定していた座標をトランスフォームコンポーネントに設定
            this->SetPos(this->_pos);
        }

        return TRUE;
    }

    void InGameScene2DActor::SetPos(const Core::Math::Vector2& in_rPos)
    {
        this->_pos = in_rPos;

        if (this->_transformHandle.Null()) return;

        // 事前に設定していた座標をトランスフォームコンポーネントに設定
        auto pTrans = this->GetComponent<Actor::TransformComponent>(this->_transformHandle);
        HE_ASSERT(pTrans);

        pTrans->SetPos(Core::Math::Vector3(this->_pos));
    }

}  // namespace InGame
