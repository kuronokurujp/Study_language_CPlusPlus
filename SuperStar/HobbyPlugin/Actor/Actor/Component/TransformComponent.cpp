#include "TransformComponent.h"

#include "Actor/Actor.h"

namespace Actor
{
    const Core::Math::Vector3 TransformComponent::GetWorldPos()
    {
        Core::Math::Vector3 pos = this->_pos;

        auto pOwnerActor = this->_pOwner;
        // 親アクターがあれば座標を取得
        if (pOwnerActor->IsChild())
        {
            Core::Common::Handle handle = pOwnerActor->GetChildTaskNode().GetParentHandle();
            while (handle.Null() == FALSE)
            {
                Object* pParentActor = this->_pOwner->GetActorByOwner(handle);
                if (pParentActor == NULL) break;

                auto pTrans = pParentActor->GetComponent<TransformComponent>();
                if (pTrans == NULL) break;

                // 親の座標を加算
                pos += pTrans->_pos;

                // 親アクターに更に親アクターがあれば処理を続ける
                if (pParentActor->IsChild())
                    handle = pParentActor->GetChildTaskNode().GetParentHandle();
                else
                    handle.Clear();
            }
        }

        return pos;
    }

    void TransformComponent::TransformLocalToWorldPos2D(Core::Math::Vector2* out,
                                                        const Core::Math::Vector2& in_rOffsetPos)
    {
        HE_ASSERT(out != NULL);

        const Core::Math::Vector3& pos = this->GetWorldPos();
        out->_fX                       = pos._fX;
        out->_fY                       = pos._fY;

        (*out) += in_rOffsetPos;
    }

    void TransformComponent::TransformLocalToWorldRect2D(Core::Math::Rect2* out,
                                                         const Core::Math::Rect2& in_rOffsetRect)
    {
        HE_ASSERT(out != NULL);

        (*out) = in_rOffsetRect;

        const Core::Math::Vector3& pos = this->GetWorldPos();
        (*out) += Core::Math::Vector2(pos._fX, pos._fY);
    }

    const Core::Math::Matrix4& TransformComponent::ComputeWorldTransform()
    {
        // 拡大・拡縮行列作成して初期行列として設定

        Core::Math::Matrix4::OutputScale(&this->_worldTransform, this->_scale._fX, this->_scale._fY,
                                         this->_scale._fZ);

        // 回転行列作成して掛け算
        Core::Math::Matrix4 rot;
        Core::Math::OutputQuaternionToMatrix4(&rot, this->_rotation);
        this->_worldTransform.Mul(rot);

        // 平行移動行列作成して掛け算
        Core::Math::Matrix4 trans;
        Core::Math::Matrix4::OutputTranslation(&trans, this->_pos);
        this->_worldTransform.Mul(trans);

        return this->_worldTransform;
    }
}  // namespace Actor
