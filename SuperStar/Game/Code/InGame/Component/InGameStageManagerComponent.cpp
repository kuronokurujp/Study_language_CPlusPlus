#include "InGameStageManagerComponent.h"

#include "InGame/Actor/Enemy/InGameZakoEnemyActor.h"
#include "InGame/Actor/Player/InGamePlayerActor.h"
#include "InGame/Component/Renderer/InGameRendererEnemyZakoComponent.h"
#include "InGame/Component/Renderer/InGameRendererUserShipComponent.h"
// ゲーム専用アセット
#include "Asset/ParamaterAssetData.h"

// ゲームキャラクター用のイベント
#include "InGame/Event/InGameEventCharacter.h"
/*
#include "actor/boss/BossStage1.h"
#include "actor/enemy/Snake.h"
#include "actor/enemy/Split.h"
#include "actor/enemy/Zako.h"
#include "actor/player/Player.h"
#include "event/data/Stage.h"
#include "game/GameSystem.h"
#include "scripting/LuaStateManager.h"
#include "system/System.h"
*/
// 利用モジュール
#include "AssetManagerModule.h"

namespace InGame
{
    /*
        static const char* s_pStageManagerScriptName[] = {
            "./resource/script/stage/stage01.lua",
        };

        static const char* s_pPlayerKeyGuideText = "a: SHOT d/s: NEXT SHOT TYPE";

    */
    //	受け取るイベントリスト
    /*
    InGameStageManagerComponent::_EVENT_FUNC InGameStageManagerComponent::m_aEventFuncList[] = {
        {"RequestStageManagerCreate", &InGameStageManagerComponent::_CreateStageEvent},
    };
    */

    InGameStageManagerComponent::InGameStageManagerComponent(
        const Core::Common::Handle& in_rViewHandle,
        const Core::Common::Handle& in_rPlayerParamaterAssetHandle)
        : Level::LevelBaseComponent()
    {
        this->_Clear();

        this->_viewHandle                 = in_rViewHandle;
        this->_playerParamaterAssetHandle = in_rPlayerParamaterAssetHandle;
    }

    Bool InGameStageManagerComponent::VBegin()
    {
        if (Level::LevelBaseComponent::VBegin() == FALSE) return FALSE;

        // TODO: ステージ制御するLuaファイルをロード
        /*
            LuaStateManager::DoFile(s_pStageManagerScriptName[m_StageIndex], "Init");

            SystemProprtyInterfaceInGame::ACTOR_HANDLE_DATA_ST& rHandle =
                SystemProprtyInterfaceInGame::GetActorHandleData();
            C_ColisionActorManager& iActorManager = C_ColisionActorManager::inst();


            m_State = _STATE_SCROLL;
            */

        auto& rParamaterAssetData =
            HE_ENGINE.ModuleManager()
                .Get<AssetManager::AssetManagerModule>()
                ->GetAsset<Game::Asset::ParamaterAssetData>(this->_playerParamaterAssetHandle);

        const UTF8* szParamaterIdName = "player_default";

        // プレイヤーアクター生成
        InGamePlayerActor::Parameter playerParamater;
        {
            playerParamater.ulife = rParamaterAssetData.GetUInt32ByIdData(szParamaterIdName, "hp");
            playerParamater.speed =
                rParamaterAssetData.GetFloat32ByIdData(szParamaterIdName, "move_spped");
            playerParamater.fInvincibleTimeSec =
                rParamaterAssetData.GetFloat32ByIdData(szParamaterIdName, "InvincibleTimeSec");
        }
        this->_playerHandle = this->AddActor<InGamePlayerActor>(playerParamater);

        // プレイヤーの外部からの初期設定
        {
            Core::Math::Vector2 size(30.0f, 30.0f);
            auto pPlayer = this->GetActor<InGamePlayerActor>(this->_playerHandle);
            // プレイヤーのレンダリングコンポーネントを追加
            auto [h, c] = pPlayer->AddComponentByHandleAndComp<InGameRendererUserShipComponent>(
                0, Actor::Component::EPriorty_Late, this->_viewHandle);

            // 位置
            pPlayer->SetPos(Core::Math::Vector2(100.0f, 100.0f));
            // サイズ
            pPlayer->SetSize(size);
        }

        // TODO: イベント追加
        {
            auto pEventModule = HE_ENGINE.ModuleManager().Get<Event::EventModule>();

            auto upStrategy =
                HE_MAKE_CUSTOM_UNIQUE_PTR((InGame::InGameCharacterEventManagerStrategy));
            this->_characterEventHandle = pEventModule->AddEventManager(std::move(upStrategy));
            HE_ASSERT(this->_characterEventHandle.Null() == FALSE);

            auto spCharacterEventListener =
                HE_MAKE_CUSTOM_SHARED_PTR((Event::EventListenerWithRegistEventFunc),
                                          HE_STR_TEXT("LevelInGameCharacterListener"),
                                          [this](Event::EventDataInterfacePtr const& in_spEventData)
                                          { return this->_HandleCharacterEvent(in_spEventData); });

            if (pEventModule->AddListener(spCharacterEventListener,
                                          INGAME_CHARACTER_EVENT_TYPE_NAME) == FALSE)
            {
                HE_ASSERT(0 && "キャラクターイベントリスナー設定に失敗");
            }
        }

        return TRUE;
    }

    Bool InGameStageManagerComponent::VEnd()
    {
        auto pEventModule = HE_ENGINE.ModuleManager().Get<Event::EventModule>();

        // 設定したイベントリスナーを解放
        pEventModule->RemoveAllListener(INGAME_CHARACTER_EVENT_TYPE_NAME);
        // 作成したイベント管理を解放
        pEventModule->RemoveEventManager(this->_characterEventHandle);

        this->RemoveActor(&this->_playerHandle);

        return LevelBaseComponent::VEnd();

        /*
            std::vector<C_EnemyActorBase*>::iterator it;
            for (it = m_aMapSettingEnemyList.begin(); it != m_aMapSettingEnemyList.end(); ++it)
            {
                SAFE_DELETE(*it);
            }
            */
    }

    Bool InGameStageManagerComponent::_HandleCharacterEvent(
        Event::EventDataInterfacePtr const& in_spEventData)
    {
        // ゲームのキャラクターイベント処理
        const Uint32 uEventHash = in_spEventData->VDataTypeHash();

        // 移動処理をする
        if (uEventHash == EventCharacterMove::EventTypeHash())
        {
            auto pEvent = reinterpret_cast<EventCharacterMove*>(in_spEventData.get());
            HE_ASSERT(pEvent != NULL);

            switch (pEvent->_eTag)
            {
                case EObjectTag_Player:
                {
                    HE_ASSERT(this->_playerHandle.Null() == FALSE);
                    auto pPlayer = this->GetActor<InGamePlayerActor>(this->_playerHandle);
                    pPlayer->Move(pEvent->_move);

                    break;
                }
                default:
                    break;
            }
        }
        // 攻撃処理をする
        else if (uEventHash == EventCharacterAttack::EventTypeHash())
        {
            auto pEvent = reinterpret_cast<EventCharacterAttack*>(in_spEventData.get());
            HE_ASSERT(pEvent != NULL);

            switch (pEvent->_eTag)
            {
                case EObjectTag_Player:
                {
                    HE_ASSERT(this->_playerHandle.Null() == FALSE);
                    auto pPlayer = this->GetActor<InGamePlayerActor>(this->_playerHandle);
                    pPlayer->Shot();

                    break;
                }
                default:
                    break;
            }
        }
        // TODO: 敵生成
        else if (uEventHash == EventCharacterPutEnemy::EventTypeHash())
        {
            auto pEvent = reinterpret_cast<EventCharacterPutEnemy*>(in_spEventData.get());
            HE_ASSERT(pEvent != NULL);

            // idはかぶらないようにする
            static Uint32 s_uEnemyId = 0;

            // TODO: タグに応じた敵を生成する
            switch (pEvent->_eEnemyTag)
            {
                case EEnemyTag::EEnemyTag_Zako:
                {
                    auto [actorHandle, pActor] =
                        this->AddActorByHandleAndActor<InGameEnemyZakoActor>();
                    auto [compHandle, pComp] =
                        pActor->AddComponentByHandleAndComp<InGameRendererEnemyZakoComponent>(
                            0, Actor::Component::EPriorty::EPriorty_Main);
                    pComp->SetViewHandle(this->_viewHandle);
                    // TODO: パラメータはテスト
                    pComp->SetSize(Core::Math::Vector2(32.0f, 32.0f));
                    pActor->SetPos(pEvent->_pos);

                    ++s_uEnemyId;
                    this->_mEnemyMap.Add(s_uEnemyId, pActor);

                    break;
                }

                default:
                    break;
            }
        }
        // TODO: プレイヤー生成

        return TRUE;
    }

    /*
            @brief	更新
    */
    /*
    bool StageManagerActor::update()
    {
        GameLib::C_GameSystem& r = GameLib::C_GameSystem::inst();

        C_ColisionActorManager& iActorManager = C_ColisionActorManager::inst();

        SystemProprtyInterfaceInGame::ACTOR_HANDLE_DATA_ST& rHandle =
            SystemProprtyInterfaceInGame::GetActorHandleData();
        C_PlayerActor* pPlayerActor = (C_PlayerActor*)iActorManager.GetData(rHandle.player);

        //	プレイヤー入力
        if (pPlayerActor != NULL)
        {
            if (r.isButtonOn('a'))
            {
                pPlayerActor->Shot();
            }
            else if (r.isButtonTriggerOn('s'))
            {
                pPlayerActor->MoveShotType(1);
            }
            else if (r.isButtonTriggerOn('d'))
            {
                pPlayerActor->MoveShotType(-1);
            }

            //	プレイヤー状態表示
            std::string ActiveShotName = pPlayerActor->GetActiveShotName();
            for (int i = 0, posX = 0; i < pPlayerActor->GetShotTypeNum(); ++i)
            {
                std::string Name = pPlayerActor->GetShotName(i);

                posX += (int)(Name.length() * 16);

                unsigned int color = 0xffffffff;
                if (ActiveShotName == Name)
                {
                    //	現在撃てる弾名
                    color = 0xff00ffff;
                }

                r.drawDebugText(Name.c_str(), 0 + posX, r.getScreenHeight() - 32, color);
            }

            //	キーガイド
            {
                r.drawDebugText(s_pPlayerKeyGuideText, 0, r.getScreenHeight() - 46);
            }

            //	プレイヤーHP表示
            char pLifeString[256] = {NULL};
            sprintf_s(pLifeString, "Life %d", pPlayerActor->GetLifeNum());
            r.drawDebugText(pLifeString, 0, r.getScreenHeight() - (46 + 16));
        }

        if (pPlayerActor == NULL)
        {
            //	自機死亡
            const char* pStageClearString = "Game Over";
            r.drawDebugText(pStageClearString,
                            (r.getScreenWidth() >> 1) - strlen(pStageClearString) * (16 >> 1),
                            (r.getScreenHeight() >> 1));

            const char* pBtnGuideString = "[a]:End";
            r.drawDebugText(pBtnGuideString,
                            (r.getScreenWidth() >> 1) - strlen(pBtnGuideString) * (16 >> 1),
                            (r.getScreenHeight() >> 1) + 32);

            if (r.isButtonTriggerOn('a') == true)
            {
                //	ゲーム終了
                SystemProprtyInterfaceInGame::SetFlgGameEnd(true);
            }
        }
        else
        //	マップ座標を動かす
        {
            switch (m_State)
            {
                case _STATE_SCROLL:
                {
                    if (m_MapXPos < m_ScrollEndXPos)
                    {
                        m_MapXPos += m_Speed;

                        //	指定した位置に到達したら敵を登録する。
                        if (!m_aMapSettingEnemyList.empty())
                        {
                            SystemProprtyInterfaceInGame::ACTOR_HANDLE_DATA_ST& rHandle =
                                SystemProprtyInterfaceInGame::GetActorHandleData();

                            //	マップ配置している敵がいる。
                            std::vector<C_EnemyActorBase*>::iterator it;
                            for (it = m_aMapSettingEnemyList.begin();
                                 it != m_aMapSettingEnemyList.end();)
                            {
                                Vec3 pos = (*it)->GetPos();
                                pos.x -= (float)m_Speed;
                                float limitXPos = (float)r.getScreenWidth() - (*it)->GetSize();
                                if (pos.x <= limitXPos)
                                {
                                    //	座標をずらして指定した範囲にいればアクターに登録
                                    Vec3 initPos                     = (*it)->GetPos();
                                    rHandle.aEnemy[rHandle.enemyMax] = iActorManager.add(*it);
                                    (*it)->VStop(false);
                                    (*it)->SetPos(initPos);

                                    ++rHandle.enemyMax;

                                    it = m_aMapSettingEnemyList.erase(it);
                                }
                                else
                                {
                                    (*it)->SetPos(pos);
                                    ++it;
                                }
                            }
                        }
                    }
                    else
                    {
                        //	ステージの端にきた。
                        for (int i = 0; i < rHandle.enemyMax; ++i)
                        {
                            iActorManager.destroy(rHandle.aEnemy[i]);
                        }
                        rHandle.enemyMax = 0;

                        if (m_bClearPoint == true)
                        {
                            //	ゲーム終了
                            m_State = _STATE_GAME_END;
                        }
                        else
                        {
                            m_BossHandle = iActorManager.add(new BossStage1);
                            m_State      = _STATE_BOSS;
                        }
                    }

                    break;
                }
                case _STATE_BOSS:
                {
                    if (iActorManager.GetData(m_BossHandle) == NULL)
                    {
                        //	終了
                        m_State = _STATE_GAME_END;
                    }

                    break;
                }
                case _STATE_GAME_END:
                {
                    char aStageEndMessageString[256] = {NULL};
                    if (m_bClearPoint == true)
                    {
                        if (SystemProprtyInterfaceInGame::GetPoint() >= m_ClearPointNum)
                        {
                            sprintf_s(aStageEndMessageString, "Stage Clear");
                        }
                        else
                        {
                            sprintf_s(aStageEndMessageString, "Game Over");
                        }
                    }
                    else
                    {
                        sprintf_s(aStageEndMessageString, "Stage Clear");
                    }

                    r.drawDebugText(aStageEndMessageString,
                                    (r.getScreenWidth() >> 1) -
                                        strlen(aStageEndMessageString) * (16 >> 1),
                                    (r.getScreenHeight() >> 1));

                    const char* pBtnGuideString = "[a]:End";
                    r.drawDebugText(pBtnGuideString,
                                    (r.getScreenWidth() >> 1) - strlen(pBtnGuideString) * (16 >> 1),
                                    (r.getScreenHeight() >> 1) + 32);

                    if (r.isButtonTriggerOn('a') == true)
                    {
                        //	ゲーム終了
                        SystemProprtyInterfaceInGame::SetFlgGameEnd(true);
                    }

                    break;
                }
            }

            //	スコア表示
            char aScoreString[256] = {NULL};
            sprintf_s(aScoreString, "Score : %d", SystemProprtyInterfaceInGame::GetPoint());
            r.drawDebugText(aScoreString, 0, 32);
        }

        return true;
    }
    */

    /*
            @brief	データ通知
    */
    /*
    void StageManagerActor::VOnCallbackSendMessage(const int in_DataType, void* in_pData)
    {
    }
    */

    /*
            @brief	登録した自分以外のアクターを取得する。
    */
    /*
    void StageManagerActor::_VOnCallbackOtherActorData(C_PaketSendActor* in_pOtherActor)
    {
    }
    */

    /*
            @brief	スクリプトで通知したデータを受け取る
    */
    /*
    void StageManagerActor::_VOnCallbackScriptEvent(lua_State* in_pLuaState)
    {
        //	スクリプトからデータを復元
        if (in_pLuaState == NULL)
        {
            return;
        }

        //	データタイプを取得
        const char* pEventTypeName = lua_tostring(in_pLuaState, 1);
        if (pEventTypeName == NULL)
        {
            return;
        }

        if (strncmp(pEventTypeName, "RequestInitStage", strlen(pEventTypeName)) == 0)
        {
            //	ステージ初期化
            StageInitPaeramEventData eventData;
            if (eventData.VBuildLuaEventData(in_pLuaState) == false)
            {
                return;
            }

            m_MapXPos       = 0;
            m_Speed         = eventData.m_Speed;
            m_W             = eventData.m_W;
            m_bClearPoint   = eventData.m_ClearPointFlg;
            m_ClearPointNum = eventData.m_ClearPoint;

            GameLib::C_GameSystem& r = GameLib::C_GameSystem::inst();
            m_ScrollEndXPos          = m_W - r.getScreenWidth();
        }
        else if (strncmp(pEventTypeName, "RequestCreateEnemy", strlen(pEventTypeName)) == 0)
        {
            //	敵作成
            StageCreateEnemyEventData eventData;

            eventData.VBuildLuaEventData(in_pLuaState);

            //	敵を作成するが、登場をしない。
            C_EnemyActorBase* pEnemy = NULL;

            Vec3 initPos((float)eventData.m_XPos, (float)eventData.m_YPos, 0.f);
            //	雑魚
            if (strncmp(eventData.m_pName, "Zako", strlen(eventData.m_pName)) == 0)
            {
                pEnemy = new C_EnemyActorZako(
                    C_EnemyActorBase::ChangeAblityTypeStringToNum(eventData.m_pTypeName));
            }
            //	蛇
            else if (strncmp(eventData.m_pName, "Snake", strlen(eventData.m_pName)) == 0)
            {
                pEnemy = new EnemyActorSnake();
            }
            //	分裂
            else if (strncmp(eventData.m_pName, "Split", strlen(eventData.m_pName)) == 0)
            {
                pEnemy = new EnemyActorSplit();
            }
            else
            {
                STRING_ASSERT(false, "生成する敵タイプが間違っています。");
            }

            if (pEnemy != NULL)
            {
                pEnemy->VStop(true);
                pEnemy->SetPos(initPos);

                m_aMapSettingEnemyList.push_back(pEnemy);
            }
        }
    }
    */

    /*
            @brief	クリア
    */
    void InGameStageManagerComponent::_Clear()
    {
        this->_playerHandle.Clear();
        this->_viewHandle.Clear();
        this->_playerParamaterAssetHandle.Clear();
        /*
            m_StageIndex    = 0;
            m_Speed         = 0;
            m_MapXPos       = 0;
            m_W             = 0;
            m_ScrollEndXPos = 0;
            m_State         = _STATE_SCROLL;
            m_bClearPoint   = false;
            m_ClearPointNum = 0;
            m_BossHandle    = 0;
            */
    }
}  // namespace InGame
