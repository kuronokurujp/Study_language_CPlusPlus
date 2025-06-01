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

        this->_spGameAsset = HE_MAKE_CUSTOM_SHARED_PTR((Game::GameAssetMap));

        auto pAssetManagerModule =
            HE_ENGINE.ModuleManager().Get<AssetManager::AssetManagerModule>();

        // TODO: アセットのロード
        // ステージのタイムラインアセットロード
        Core::Common::Handle stageTimelineParamaterAssetHandle;
        {
            constexpr auto szParamaterAssetName = HE_STR_TEXT("StageTimelineParamater");
            stageTimelineParamaterAssetHandle =
                pAssetManagerModule->Load<Game::Asset::ParamaterAssetData>(
                    szParamaterAssetName,
                    Core::File::Path(HE_STR_TEXT("Paramater/Stage_Timeline_01.json")));
            this->_spGameAsset->Add(szParamaterAssetName, stageTimelineParamaterAssetHandle);
        }

        // プレイヤーのアセットロード
        Core::Common::Handle playerParamaterAssetHandle;
        {
            // パラメータ
            {
                constexpr auto szPlayerParamaterAssetName = HE_STR_TEXT("PlayerParamater");
                playerParamaterAssetHandle =
                    pAssetManagerModule->Load<Game::Asset::ParamaterAssetData>(
                        szPlayerParamaterAssetName,
                        Core::File::Path(HE_STR_TEXT("Paramater/Player.json")));
                this->_spGameAsset->Add(szPlayerParamaterAssetName, playerParamaterAssetHandle);
            }
        }

        // 敵のアセットロード
        Core::Common::Handle enemyParamaterAssetHandle;
        {
            // パラメータ
            {
                constexpr auto szEnemeyParamaterAssetName = HE_STR_TEXT("EnemyParamater");
                enemyParamaterAssetHandle =
                    pAssetManagerModule->Load<Game::Asset::ParamaterAssetData>(
                        szEnemeyParamaterAssetName,
                        Core::File::Path(HE_STR_TEXT("Paramater/Enemy.json")));

                this->_spGameAsset->Add(szEnemeyParamaterAssetName, enemyParamaterAssetHandle);
            }

            // TODO: 使用するLuaスクリプトテキストファイルをロード
            {
                Game::Asset::ParamaterAssetData& rParamAsset =
                    pAssetManagerModule->GetAsset<Game::Asset::ParamaterAssetData>(
                        enemyParamaterAssetHandle);

                HE::Uint32 uIndex = 0;
                while (rParamAsset.IsIndex(uIndex))
                {
                    rParamAsset.OutputStringByIndex(&Core::Common::g_szTempFixedString128, uIndex,
                                                    HE_STR_U8_TEXT("lua_file_name"));

                    Core::Common::g_szTempFixedString1024 = Core::Common::g_szTempFixedString128;
                    Core::Common::g_szTempFixedString1024 += HE_STR_TEXT(".lua");
                    Core::File::Path path(HE_STR_TEXT("Lua"), HE_STR_TEXT("Enemy"),
                                          Core::Common::g_szTempFixedString1024.Str());

                    // TODO: Luaスクリプトファイルをテキストとしてロード
                    // すでにロードしているならそれを使う
                    auto handle = pAssetManagerModule->GetAssetHandle(path);
                    if (handle.Null())
                    {
                        handle = pAssetManagerModule->Load<AssetManager::AssetDataText>(
                            Core::Common::g_szTempFixedString1024.Str(), path);

                        this->_spGameAsset->Add(Core::Common::g_szTempFixedString128, handle);
                    }

                    ++uIndex;
                }
            }
        }

        // TODO: エフェクトロード
        {
        }

        // サウンドロード
        {
        }

        // ユーザー共通入力割り当て設定
        {
            auto pInputModule = HE_ENGINE.ModuleManager().Get<EnhancedInput::EnhancedInputModule>();
            pInputModule->AddAction(Local::mInputActionByPlay);
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

                                pEventModule->QueueEvent(spEvent, EVENT_TYPE_INGAME_CHARACTER);
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

                                pEventModule->QueueEvent(spEvent, EVENT_TYPE_INGAME_CHARACTER);
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
        // オブジェクトのイベント通知などステージに特化したコンポーネント
        {
            auto [handle, pComp] =
                this->AddComponentByHandleAndComp<InGame::InGameStageManagerComponent>(
                    0, Actor::Component::EPriorty::EPriorty_Main, Game::g_scene2DHandle,
                    playerParamaterAssetHandle, enemyParamaterAssetHandle,
                    stageTimelineParamaterAssetHandle, this->_spGameAsset);
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
            pInputModule->RemoveAction(Local::mInputActionByPlay);
        }

        this->_spGameAsset.reset();

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

    void LevelInGame::VProcessInput(const EnhancedInput::InputMap& in_rInputMap)
    {
        Level::Node::VProcessInput(in_rInputMap);

        auto pEventModule = HE_ENGINE.ModuleManager().Get<Event::EventModule>();

        Core::Math::Vector2 move;
        if (in_rInputMap.Contains(Local::szInputMoveUp))
        {
            move += Core::Math::Vector2(0.0f, -1.0f);
        }
        else if (in_rInputMap.Contains(Local::szInputMoveDown))
        {
            move += Core::Math::Vector2(0.0f, 1.0f);
        }

        if (in_rInputMap.Contains(Local::szInputMoveLeft))
        {
            move += Core::Math::Vector2(-1.0f, 0.0f);
        }
        else if (in_rInputMap.Contains(Local::szInputMoveRight))
        {
            move += Core::Math::Vector2(1.0f, 0.0f);
        }

        if (in_rInputMap.Contains(Local::szInputShot))
        {
            auto pStageComponent = this->GetComponent<InGame::InGameStageManagerComponent>(
                this->_stageManagerComponentHandle);

            auto rHandle = pStageComponent->GetPlayerHandle();
            if (rHandle.Null() == FALSE)
            {
                auto spEvent = HE_MAKE_CUSTOM_SHARED_PTR((InGame::EventCharacterAttack), 0,
                                                         InGame::EObjectTag_Player, rHandle);
                pEventModule->QueueEvent(spEvent, EVENT_TYPE_INGAME_CHARACTER);
            }
        }

        if (move.IsZero() == FALSE)
        {
            auto pStageComponent = this->GetComponent<InGame::InGameStageManagerComponent>(
                this->_stageManagerComponentHandle);

            auto rHandle = pStageComponent->GetPlayerHandle();
            if (rHandle.Null() == FALSE)
            {
                move.Normalize();
                auto spEvent = HE_MAKE_CUSTOM_SHARED_PTR((InGame::EventCharacterMove), 0,
                                                         InGame::EObjectTag_Player, rHandle, move);
                pEventModule->QueueEvent(spEvent, EVENT_TYPE_INGAME_CHARACTER);
            }
        }
    }

}  // namespace Level
