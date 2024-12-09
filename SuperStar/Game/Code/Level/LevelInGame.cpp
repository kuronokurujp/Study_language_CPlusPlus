#include "LevelInGame.h"

#include "Common.h"
#include "Engine/Common/Hash.h"
#include "Engine/Engine.h"
#include "Engine/Math/Vector2.h"

// ゲーム専用アセット
#include "Asset/ParamaterAssetData.h"

// インゲーム専用コンポーネント一覧
#include "InGame/Component/InGameBulletManagerComponent.h"
#include "InGame/Component/InGameCollisionComponent.h"
#include "InGame/Component/InGameStageManagerComponent.h"
#include "InGame/Component/InGameSystemComponent.h"
#include "InGame/InGameTag.h"

// 子レベル一覧
#include "LevelInGame/LevelInGame_BG.h"

// イベント一覧
#include "InGame/Event/InGameEventCharacter.h"

// 利用モジュール
#include "AssetManagerModule.h"
#include "EnhancedInputModule.h"
#include "EventModule.h"
#include "RenderModule.h"

namespace Level
{
    namespace Local
    {
        // プレイヤーのユーザー入力
        static const HE::Char* szInputMoveUp    = HE_STR_TEXT("Player_MoveUp");
        static const HE::Char* szInputMoveLeft  = HE_STR_TEXT("Player_MoveLeft");
        static const HE::Char* szInputMoveDown  = HE_STR_TEXT("Player_MoveDown");
        static const HE::Char* szInputMoveRight = HE_STR_TEXT("Player_MoveRight");
        static const HE::Char* szInputShot      = HE_STR_TEXT("Player_Shot");

        static const EnhancedInput::ActionMap mInputActionByPlay =
            {{szInputMoveUp, EnhancedInput::ActionData({Platform::EKeyboard::EKeyboard_W})},
             {szInputMoveLeft, EnhancedInput::ActionData({Platform::EKeyboard::EKeyboard_A})},
             {szInputMoveDown, EnhancedInput::ActionData({Platform::EKeyboard::EKeyboard_S})},
             {szInputMoveRight, EnhancedInput::ActionData({Platform::EKeyboard::EKeyboard_D})},
             {szInputShot, EnhancedInput::ActionData({Platform::EKeyboard::EKeyboard_SPACE})}};

    };  // namespace Local

    HE::Bool LevelInGame::VBegin()
    {
        const HE::Bool bRet = Node::VBegin();
        HE_ASSERT(bRet);

        auto pAssetManagerModule =
            HE_ENGINE.ModuleManager().Get<AssetManager::AssetManagerModule>();

        // TODO: アセットのロード
        // ステージのタイムラインアセットロード
        Core::Common::Handle stageTimelineParamaterAssetHandle;
        {
            const auto szParamaterAssetName = HE_STR_TEXT("StageTimelineParamater");
            stageTimelineParamaterAssetHandle =
                pAssetManagerModule->Load<Game::Asset::ParamaterAssetData>(
                    szParamaterAssetName,
                    Core::File::Path(HE_STR_TEXT("Paramater/Stage_Timeline_01.json")));
            this->_mGameAsset.Add(szParamaterAssetName, stageTimelineParamaterAssetHandle);
        }

        // プレイヤーのアセットロード
        Core::Common::Handle playerParamaterAssetHandle;
        {
            const auto szPlayerParamaterAssetName = HE_STR_TEXT("PlayerParamater");
            playerParamaterAssetHandle = pAssetManagerModule->Load<Game::Asset::ParamaterAssetData>(
                szPlayerParamaterAssetName, Core::File::Path(HE_STR_TEXT("Paramater/Player.json")));
            this->_mGameAsset.Add(szPlayerParamaterAssetName, playerParamaterAssetHandle);
        }

        // 敵のアセットロード
        Core::Common::Handle enemyParamaterAssetHandle;
        {
            const auto szEnemeyParamaterAssetName = HE_STR_TEXT("EnemyParamater");
            enemyParamaterAssetHandle = pAssetManagerModule->Load<Game::Asset::ParamaterAssetData>(
                szEnemeyParamaterAssetName, Core::File::Path(HE_STR_TEXT("Paramater/Enemy.json")));

            this->_mGameAsset.Add(szEnemeyParamaterAssetName, enemyParamaterAssetHandle);
        }

        // ユーザー共通入力割り当て設定
        {
            auto pInputModule = HE_ENGINE.ModuleManager().Get<EnhancedInput::EnhancedInputModule>();
            pInputModule->AddCommonMappingAction(Local::mInputActionByPlay);
        }

        // 背景のレベル追加
        this->AddLevel<LevelInGame_BG>();

        // インゲームのシステムコンポーネントを追加
        {
            this->_systemComponentHandle = this->AddComponent<InGame::InGameSystemComponent>(
                0, Actor::Component::EPriorty::EPriorty_Main);
            auto pSystemComponent =
                this->GetComponent<InGame::InGameSystemComponent>(this->_systemComponentHandle);
            pSystemComponent->SetFlgGameEnd(FALSE);
            pSystemComponent->SetPoint(0);
        }

        // 味方や敵の弾を管理するオブジェクトとコンポーネント追加
        {
            auto bulletManagerActorHandle = this->AddActor<Actor::Object>();
            auto pActor                   = this->GetActor<Actor::Object>(bulletManagerActorHandle);

            // 弾を描画ハンドルを渡す
            auto [handle, pComp] =
                pActor->AddComponentByHandleAndComp<InGame::InGameBulletManagerComponent>(
                    0, Actor::Component::EPriorty::EPriorty_Main, Game::g_scene2DHandle);
            HE_ASSERT(handle.Null() == FALSE);

            pComp->SetCollisionHashCode(HE_STR_TEXT("Bullet"));

            // 弾が当たった時のヒットアクション
            pComp->SetHitAction(
                [](const InGame::CollisionData& in_rSelf, const InGame::CollisionData& in_rTarget)
                {
                    auto pEventModule = HE_ENGINE.ModuleManager().Get<Event::EventModule>();
                    switch (in_rTarget.uHashCode)
                    {
                        case InGame::EObjectTag::EObjectTag_Player:
                        {
                            if (in_rSelf.uHashCode == InGame::EObjectTag::EObjectTag_Enemy)
                            {
                                // TODO: プレイヤーへダメージイベント通知
                                auto spEvent =
                                    HE_MAKE_CUSTOM_SHARED_PTR((InGame::EventCharacterDamage), 0,
                                                              InGame::EObjectTag::EObjectTag_Player,
                                                              in_rTarget.ulMetaData, 1);

                                pEventModule->QueueEvent(spEvent);
                            }

                            break;
                        }
                        case InGame::EObjectTag::EObjectTag_Enemy:
                        {
                            if (in_rSelf.uHashCode == InGame::EObjectTag::EObjectTag_Player)
                            {
                                // TODO: 敵へダメージイベント通知
                                auto spEvent =
                                    HE_MAKE_CUSTOM_SHARED_PTR((InGame::EventCharacterDamage), 0,
                                                              InGame::EObjectTag::EObjectTag_Enemy,
                                                              in_rTarget.ulMetaData, 1);

                                pEventModule->QueueEvent(spEvent);
                            }

                            break;
                        }
                        default:
                            break;
                    }

                    HE_LOG_LINE(HE_STR_TEXT("Hit Bullet"));
                    return TRUE;
                });
        }

        // インゲームのステージコンポーネント追加
        {
            auto [handle, pComp] =
                this->AddComponentByHandleAndComp<InGame::InGameStageManagerComponent>(
                    0, Actor::Component::EPriorty::EPriorty_Main, Game::g_scene2DHandle,
                    playerParamaterAssetHandle, enemyParamaterAssetHandle,
                    stageTimelineParamaterAssetHandle);
            this->_stageManagerComponentHandle = handle;
        }

        return TRUE;
    }

    HE::Bool LevelInGame::VEnd()
    {
        // ロードしたアセットを解放
        {
            auto pAssetManagerModule =
                HE_ENGINE.ModuleManager().Get<AssetManager::AssetManagerModule>();
        }

        // 専用の入力アクションを外す
        {
            auto pInputModule = HE_ENGINE.ModuleManager().Get<EnhancedInput::EnhancedInputModule>();
            pInputModule->RemoveCommonMappingAction(Local::mInputActionByPlay);
        }

        return Node::VEnd();
    }

    void LevelInGame::VUpdate(const HE::Float32 in_fDt)
    {
        Node::VUpdate(in_fDt);

        auto pSystemComponent =
            this->GetComponent<InGame::InGameSystemComponent>(this->_systemComponentHandle);
        {
            if (pSystemComponent->IsGameEnd())
            {
                // TODO: ゲームオーバー
            }
        }

        // コリジョン処理をする
        InGame::CollisionAll();
    }

    void LevelInGame::_VProcessInput(const EnhancedInput::InputMap* in_pInputMap)
    {
        HE_ASSERT(in_pInputMap);
        Level::Node::_VProcessInput(in_pInputMap);

        auto pEventModule = HE_ENGINE.ModuleManager().Get<Event::EventModule>();

        Core::Math::Vector2 move;
        if (in_pInputMap->Contains(Local::szInputMoveUp))
        {
            move += Core::Math::Vector2(0.0f, -1.0f);
        }
        else if (in_pInputMap->Contains(Local::szInputMoveDown))
        {
            move += Core::Math::Vector2(0.0f, 1.0f);
        }

        if (in_pInputMap->Contains(Local::szInputMoveLeft))
        {
            move += Core::Math::Vector2(-1.0f, 0.0f);
        }
        else if (in_pInputMap->Contains(Local::szInputMoveRight))
        {
            move += Core::Math::Vector2(1.0f, 0.0f);
        }

        if (in_pInputMap->Contains(Local::szInputShot))
        {
            auto spEvent = HE_MAKE_CUSTOM_SHARED_PTR((InGame::EventCharacterAttack), 0,
                                                     InGame::EObjectTag_Player, 0);
            pEventModule->QueueEvent(spEvent);
        }

        if (move.IsZero() == FALSE)
        {
            move.Normalize();
            auto spEvent = HE_MAKE_CUSTOM_SHARED_PTR((InGame::EventCharacterMove), 0,
                                                     InGame::EObjectTag_Player, 0, move);
            pEventModule->QueueEvent(spEvent);
        }
    }

}  // namespace Level
