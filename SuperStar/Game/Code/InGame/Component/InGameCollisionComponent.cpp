#include "InGameCollisionComponent.h"

#include "Actor/Actor.h"
#include "Actor/Component/TransformComponent.h"
#include "Engine/Common/Hash.h"

namespace InGame
{
    namespace Local
    {
        // コリジョンリスト
        static Core::Common::FixedVector<InGameCollisionComponent*, 1024> vCollision;

        /// <summary>
        /// ２次元の円同士の衝突ヒット
        /// </summary>
        static HE::Bool IsHitWithCircle2D(const CollisionData& in_rSelfColData,
                                          const CollisionData& in_rTargetColData)
        {
            auto len = Core::Math::Vector2::Distance(in_rSelfColData._data.circle2D.pos,
                                                     in_rTargetColData._data.circle2D.pos);

            return (len <= (in_rSelfColData._data.circle2D.fRadius +
                            in_rTargetColData._data.circle2D.fRadius));
        }

        /// <summary>
        /// コンポーネント同士が持っている衝突データを使った衝突処理
        /// </summary>
        static void ProcessCollision(InGameCollisionComponent& in_rSelf,
                                     InGameCollisionComponent& in_rTarget)
        {
            auto uSelfColCount = in_rSelf.VColCount();
            CollisionData selfColData;
            CollisionData targetColData;

            for (HE::Uint32 selfColIndex = 0; selfColIndex < uSelfColCount; ++selfColIndex)
            {
                if (in_rSelf.VOutputColData(&selfColData, selfColIndex) == FALSE) continue;

                auto uTargetColCount = in_rTarget.VColCount();
                for (HE::Uint32 targetColIndex = 0; targetColIndex < uTargetColCount;
                     ++targetColIndex)
                {
                    if (in_rTarget.VOutputColData(&targetColData, targetColIndex) == FALSE)
                        continue;

                    // 異なるコンポーネントだがハッシュが同じなら属性が同じなので衝突できない
                    if (selfColData.uHashCode == targetColData.uHashCode) continue;

                    switch (selfColData.eType)
                    {
                        case ECollisionType::ECollisionType_Circle2D:
                        {
                            switch (targetColData.eType)
                            {
                                case ECollisionType::ECollisionType_Circle2D:
                                {
                                    // 2Dの円同士の当たり判定
                                    if (IsHitWithCircle2D(selfColData, targetColData))
                                    {
                                        in_rSelf.VOnHit(selfColData, targetColData);
                                        in_rTarget.VOnHit(targetColData, selfColData);
                                    }
                                    break;
                                }
                                default:
                                    HE_ASSERT(FALSE && "コリジョンタイプがないのでエラー");
                            }
                            break;
                        }
                        default:
                            HE_ASSERT(FALSE && "コリジョンタイプがないのでエラー");
                    }
                }
            }
        }
    }  // namespace Local

    void CollisionAll()
    {
        auto size = Local::vCollision.Size();
        if (size <= 1) return;

        for (HE::Uint32 uSelfIndex = 0; uSelfIndex < size; ++uSelfIndex)
        {
            auto pSelfCollision = Local::vCollision[uSelfIndex];
            for (HE::Uint32 uTargetIndex = uSelfIndex + 1; uTargetIndex < size; ++uTargetIndex)
            {
                auto pTargetCollision = Local::vCollision[uTargetIndex];

                // コリジョン処理
                Local::ProcessCollision(*pSelfCollision, *pTargetCollision);
            }
        }
    }

    HE::Bool InGameCollisionComponent::VBegin()
    {
        if (Actor::Component::VBegin() == FALSE) return FALSE;

        Local::vCollision.PushBack(this);

        return TRUE;
    }

    HE::Bool InGameCollisionComponent::VEnd()
    {
        this->_hitAction = NULL;

        if (Actor::Component::VEnd() == FALSE) return FALSE;

        Local::vCollision.Remove(this);

        return TRUE;
    }

    HE::Bool InGameCollisionComponent::VOnHit(const CollisionData& in_rSelfColData,
                                              const CollisionData& in_rHitColData)
    {
        if (this->_hitAction == NULL) return TRUE;

        if (this->_hitAction(in_rSelfColData, in_rHitColData))
        {
            // アクションでコリジョン成功かどうかを決める事ができる
            return TRUE;
        }

        return FALSE;
    }

    void InGameCollisionComponent::SetCollisionHashCode(const HE::Char* in_szName)
    {
        this->SetCollisionHashCode(Core::Common::HashName(in_szName));
    }

    void InGameCollisionComponent::SetCollisionHashCode(const HE::Uint32 in_uHashCode)
    {
        this->_uHashCode = in_uHashCode;
    }

    void InGameCollisionComponent::SetMetaData(const HE::Uint64 in_ulMetaData)
    {
        this->_ulMetaData = in_ulMetaData;
    }

    HE::Bool InGameCircleCollision2DComponent::VOutputColData(CollisionData* out,
                                                              const HE::Uint32 in_uColIndex)
    {
        auto pTrans = this->_pOwner->GetComponent<Actor::TransformComponent>();
        HE_ASSERT(pTrans);

        auto worldPos               = pTrans->GetWorldPos();
        out->_data.circle2D.pos     = Core::Math::Vector2(worldPos._fX, worldPos._fY);
        out->_data.circle2D.fRadius = this->_fRadius;
        out->eType                  = ECollisionType::ECollisionType_Circle2D;
        out->uHashCode              = this->_uHashCode;
        out->ulMetaData             = this->_ulMetaData;

        return TRUE;
    }

}  // namespace InGame
