#include "InGamePlayerActor.h"

#include "InGame/Component//Renderer/InGameRendererUserShipComponent.h"
#include "InGame/Component/InGameCollisionComponent.h"
#include "InGame/Component/InGameShotComponent.h"
#include "InGame/InGameTag.h"
#include "InGame/Shot/InGameShotStrategy.h"

namespace InGame
{
    InGamePlayerActor::InGamePlayerActor(const Parameter& in_defaultParam) : InGameScene2DActor()
    {
        this->_Clear();

        this->_defaultParameter = in_defaultParam;
        this->_parameter        = this->_defaultParameter;
    }

    HE::Bool InGamePlayerActor::VBegin()
    {
        if (InGameScene2DActor::VBegin() == FALSE) return FALSE;

        // 当たり判定コンポーネント追加
        {
            auto [handle, pComponent] =
                this->AddComponentByHandleAndComp<InGameCircleCollision2DComponent>(
                    0, Actor::Component::EPriorty::EPriorty_Main);
            HE_ASSERT(handle.Null() == FALSE);

            pComponent->SetRadius(HE_MIN(this->_size._fX, this->_size._fY));
            pComponent->SetCollisionHashCode(InGame::EObjectTag::EObjectTag_Player);
            pComponent->SetMetaData(this->Handle());

            //  TODO: 当たった時の処理を追加
            pComponent->SetHitAction(
                [](const CollisionData& in_rSelf, const CollisionData& in_rHit)
                {
                    HE_LOG_LINE(HE_STR_TEXT("Hit"));
                    return TRUE;
                });

            this->_collisoinHandle = handle;
        }

        // 弾を打つ機能一覧作成
        // TODO: 各機能を実装する
        {
            this->_mShotStrategy.Add(HE_STR_TEXT("Normal"),
                                     HE_MAKE_CUSTOM_SHARED_PTR((InGameShotStrategyNormalBullet)));
            this->_mShotStrategy.Add(HE_STR_TEXT("Laser"),
                                     HE_MAKE_CUSTOM_SHARED_PTR((InGameShotStrategyNormalBullet)));
            this->_mShotStrategy.Add(HE_STR_TEXT("Homing"),
                                     HE_MAKE_CUSTOM_SHARED_PTR((InGameShotStrategyNormalBullet)));
            this->_mShotStrategy.Add(HE_STR_TEXT("4Way"),
                                     HE_MAKE_CUSTOM_SHARED_PTR((InGameShotStrategyNormalBullet)));
        }

        // 弾を打つコンポーネントを追加
        {
            this->_shotHandle =
                this->AddComponent<InGameShotComponent>(2,
                                                        Actor::Component::EPriorty::EPriorty_Main);
            HE_ASSERT(this->_shotHandle.Null() == FALSE);

            auto pShotComponent = this->GetComponent<InGameShotComponent>(this->_shotHandle);
            pShotComponent->SetStrategy(this->_mShotStrategy[HE_STR_TEXT("Normal")]);
        }

        return TRUE;
    }

    HE::Bool InGamePlayerActor::VEnd()
    {
        // 弾の機能解放
        for (auto itr = this->_mShotStrategy.Begin(); itr != this->_mShotStrategy.End(); ++itr)
        {
            itr->_data.reset();
        }

        return InGameScene2DActor::VEnd();
    }

    void InGamePlayerActor::VUpdate(const HE::Float32 in_fDt)
    {
        InGameScene2DActor::VUpdate(in_fDt);

        // 移動情報による座標更新
        {
            Core::Math::Vector2 newPos;
            newPos.SetAdd(this->_pos, this->_move);
            this->_move.Clear();

            this->SetPos(newPos);
        }
    }

    void InGamePlayerActor::SetSize(const Core::Math::Vector2& in_rSize)
    {
        auto pRenderer = this->GetComponent<InGameRendererUserShipComponent>();
        if (pRenderer)
        {
            pRenderer->VSetSize(in_rSize);
        }

        this->_size = in_rSize;
    }

    void InGamePlayerActor::Move(const Core::Math::Vector2& in_rMove)
    {
        this->_move.Clear();
        this->_move.Madd(in_rMove, this->_parameter.speed);
    }

    void InGamePlayerActor::Shot()
    {
        HE_ASSERT(this->_transformHandle.Null() == FALSE);
        HE_ASSERT(this->_shotHandle.Null() == FALSE);

        auto pShotComponent = this->GetComponent<InGameShotComponent>(this->_shotHandle);
        HE_ASSERT(pShotComponent);

        auto pTrans = this->GetComponent<Actor::TransformComponent>(this->_transformHandle);
        HE_ASSERT(pTrans);

        auto pCollision =
            this->GetComponent<InGameCircleCollision2DComponent>(this->_collisoinHandle);
        HE_ASSERT(pCollision);

        auto pos = pTrans->GetWorldPos();

        // 弾を打つ
        // 自身が打った弾は接触しないようにコリジョンハッシュコードを設定
        pShotComponent->Shot(Core::Math::Vector2(pos._fX, pos._fY), Core::Math::Vector2(1.0f, 0.0f),
                             pCollision->HashCode());
    }

    HE::Bool InGamePlayerActor::Damage(const HE::Sint32 in_sDamage)
    {
        this->_parameter.ulife -= in_sDamage;
        if (this->_parameter.ulife <= 0) return TRUE;

        return FALSE;
    }

}  // namespace InGame
