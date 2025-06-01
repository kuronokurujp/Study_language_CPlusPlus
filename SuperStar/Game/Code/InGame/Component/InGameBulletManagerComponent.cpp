#include "InGameBulletManagerComponent.h"

#include "Engine/Engine.h"
#include "Engine/Math/Rect2.h"

// 弾制御一覧
#include "InGame/Bullet/InGameBulletNormal.h"
// 弾を打つイベント
#include "InGame/Event/InGameEventShot.h"

// 利用モジュール
// #include "Engine/Platform/PlatformModule.h"
#include "RenderModule.h"

namespace InGame
{
    InGameBulletManagerComponent::InGameBulletManagerComponent(Core::Common::Handle in_rViewHandle)
        : InGameCollisionComponent()
    {
        this->_viewHandle = in_rViewHandle;
    }

    void InGameBulletManagerComponent::VSetup(const HE::Bool in_bReleaseMem)
    {
        InGame::InGameCollisionComponent::VSetup(in_bReleaseMem);
        this->_mBulletStrategy.Clear();
    }

    HE::Bool InGameBulletManagerComponent::VBegin()
    {
        // 利用する弾のアルゴリズムを登録
        {
            auto upStrategy = HE_MAKE_CUSTOM_UNIQUE_PTR((InGame::InGameBulletNormalStrategy));
            this->AddStrategy(std::move(upStrategy));
        }

        // 弾を打つイベント管理を追加
        {
            auto pEventModule = HE_ENGINE.ModuleManager().Get<Event::EventModule>();

            auto shotEventListener =
                HE_MAKE_CUSTOM_SHARED_PTR((Event::EventListenerWithRegistEventFunc),
                                          HE_STR_TEXT("LevelInGameShotListener"),
                                          [this](Event::EventDataInterfacePtr const& in_spEventData)
                                          { return this->_HandleEvent(in_spEventData); });

            this->_eventListeningHandle =
                pEventModule->AddListener(shotEventListener, EVENT_TYPE_INGAME_SHOT);
            if (this->_eventListeningHandle.Null())
            {
                HE_ASSERT(0 && "イベントリスナー設定に失敗");
            }
        }

        return InGame::InGameCollisionComponent::VBegin();
    }

    HE::Bool InGameBulletManagerComponent::VEnd()
    {
        auto pEventModule = HE_ENGINE.ModuleManager().Get<Event::EventModule>();

        // 設定したイベントリスナーを解放
        pEventModule->RemoveListener(this->_eventListeningHandle);

        this->_mBulletStrategy.Clear();
        return InGame::InGameCollisionComponent::VEnd();
    }

    void InGameBulletManagerComponent::VUpdate(const HE::Float32 in_fDt)
    {
        InGame::InGameCollisionComponent::VUpdate(in_fDt);
        // 弾の生存範囲内のデータを作る
        Core::Math::Rect2 activeScreenInSide;
        {
            // auto pPlatform = HE_ENGINE.ModuleManager().Get<Render Platform::PlatformModule>();
            // auto pScreen   = pPlatform->VScreen();
            auto pRender   = HE_ENGINE.ModuleManager().Get<Render::RenderModule>();
            auto pViewPort = pRender->GetViewPort(this->_viewHandle);
            //            const Platform::ScreenSceneView2DEnvironment&& rrSceneView2DEnv =
            //                pScreen->VGetEnvBySceneView2D(this->_viewHandle);

            HE::Float32 fW =
                static_cast<HE::Float32>(pViewPort->Width());  // rrSceneView2DEnv._uWidth);
            HE::Float32 fH =
                static_cast<HE::Float32>(pViewPort->Height());  // rrSceneView2DEnv._uHeight);

            const HE::Float32 fSideLength = 100.0f;
            activeScreenInSide.SetPosition(-fSideLength, -fSideLength, 2 * fSideLength + fW,
                                           2 * fSideLength + fH, Core::Math::EAnchor_Left);
        }

        this->_ForEachObject(
            [this, activeScreenInSide](InGame::InGameBulletObject* in_pObject,
                                       InGame::InGameBulletStrategyInterface* in_pStrategy)
            {
                if (in_pObject->bKill) return FALSE;

                // 更新失敗したらワークから外す
                if (in_pStrategy->VUpdate(&in_pObject->work, this->_viewHandle) == FALSE)
                {
                    return FALSE;
                }
                // 画面外かチェックして画面外ならワークから外す
                else if (in_pStrategy->VIsScreenInSide(&in_pObject->work, activeScreenInSide) ==
                         FALSE)
                {
                    return FALSE;
                }

                return TRUE;
            });
    }

    HE::Bool InGameBulletManagerComponent::VOnHit(const CollisionData& in_rSelfColData,
                                                  const CollisionData& in_rHitColData)
    {
        // コリジョン成功か失敗か
        if (InGameCollisionComponent::VOnHit(in_rSelfColData, in_rHitColData) == FALSE)
        {
            return FALSE;
        }

        // 接触した場合は弾を消すフラグを立てる
        this->_vBullet.GetPtr(in_rSelfColData.ulMetaData)->bKill = TRUE;

        return TRUE;
    }

    HE::Bool InGameBulletManagerComponent::MakeObject(
        Core::Memory::UniquePtr<InGameBulletFactoryInterface> in_upFactory)
    {
        InGameBulletObject obj;

        HE_STR_COPY_S(obj.aName, HE_ARRAY_NUM(obj.aName), in_upFactory->VName(),
                      HE_STR_LENGTH(in_upFactory->VName()));

        ::memset(&obj.work, 0, HE_ARRAY_SIZE(obj.work));
        in_upFactory->VConfiguration(&obj.work);

        this->_vBullet.PushBack(obj);

        return TRUE;
    }

    HE::Bool InGameBulletManagerComponent::AddStrategy(
        Core::Memory::UniquePtr<InGameBulletStrategyInterface> in_upStrategy)
    {
        if (this->_mBulletStrategy.Contains(in_upStrategy->VName()))
        {
            HE_ASSERT(0);
            return FALSE;
        }

        auto szName = in_upStrategy->VName();
        this->_mBulletStrategy.AddByMoveData(szName, std::move(in_upStrategy));

        return TRUE;
    }

    HE::Bool InGameBulletManagerComponent::VOutputColData(CollisionData* out,
                                                          const HE::Uint32 in_uColIndex)
    {
        auto pBullet = this->_vBullet.GetPtr(in_uColIndex);

        auto itr = this->_mBulletStrategy.FindKey(pBullet->aName);
        if (this->_mBulletStrategy.End() == itr) return FALSE;

        out->ulMetaData = in_uColIndex;

        // ストラテジー毎にコリジョンデータを作成
        return itr->_data->VOutputCollisionData(out, &pBullet->work);
    }

    void InGameBulletManagerComponent::_ForEachObject(
        std::function<HE::Bool(InGame::InGameBulletObject*, InGame::InGameBulletStrategyInterface*)>
            in_func)
    {
        HE::Sint32 iWorkSize = static_cast<HE::Sint32>(this->_vBullet.Size());
        if (iWorkSize <= 0) return;

        for (HE::Sint32 i = iWorkSize - 1; 0 <= i; --i)
        {
            auto pObject = &this->_vBullet[i];

            if (this->_mBulletStrategy.Contains(pObject->aName) == FALSE) continue;

            auto itr = this->_mBulletStrategy.FindKey(pObject->aName);
            HE_ASSERT(itr->_data->VWorkSize() <= sizeof(pObject->work));

            if (in_func(pObject, itr->_data.get()) == FALSE)
            {
                this->_vBullet.RemoveAt(i);
            }
        }
    }

    /// <summary>
    /// リスナーがイベント受け取ったかどうか
    /// </summary>
    HE::Bool InGameBulletManagerComponent::_HandleEvent(
        Event::EventDataInterfacePtr const& in_spEventData)
    {
        // ゲームのイベント処理を記述

        // 通常弾を打つ
        if (in_spEventData->VEventHash() == EventShotNormalBullet::EventTypeHash())
        {
            // 通常弾の発射処理

            EventShotNormalBullet* pEvent =
                reinterpret_cast<EventShotNormalBullet*>(in_spEventData.get());
            HE_ASSERT(pEvent != NULL);

            auto upFactor = HE_MAKE_CUSTOM_UNIQUE_PTR((InGameBulletNormalFactory), pEvent->_pos,
                                                      pEvent->_dir, pEvent->_uCollisionHashCode);

            this->MakeObject(std::move(upFactor));
        }

        return TRUE;
    }
}  // namespace InGame
