#include "InGameScene2DActor.h"

#include "Actor/Component/TransformComponent.h"

namespace InGame
{
    InGameScene2DActor::InGameScene2DActor() : Actor::Object()
    {
        this->_Clear();
    }

    HE::Bool InGameScene2DActor::VBegin()
    {
        if (Actor::Object::VBegin() == FALSE) return FALSE;

        // 座標関連のコンポーネント追加
        {
            this->_transformHandle = this->AddComponent<Actor::TransformComponent>(
                1, Actor::Component::EPriorty::EPriorty_Main);
            HE_ASSERT((this->_transformHandle.Null() == FALSE) &&
                      "トランスフォームコンポーネントの追加失敗");

            // 事前に設定していた座標/方向をトランスフォームコンポーネントに設定
            this->SetPos(this->_pos);
            this->SetDir(this->_dir);
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

    void InGameScene2DActor::SetDir(const Core::Math::Vector2& in_rDir)
    {
        this->_dir = in_rDir;

        if (this->_transformHandle.Null()) return;

        // 事前に設定していた座標をトランスフォームコンポーネントに設定
        auto pTrans = this->GetComponent<Actor::TransformComponent>(this->_transformHandle);
        HE_ASSERT(pTrans);

        pTrans->SetDir(Core::Math::Vector3(this->_dir));
    }

}  // namespace InGame
