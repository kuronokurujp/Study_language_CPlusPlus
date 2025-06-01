#pragma once

#include "Common.h"
#include "InGame/Event/InGameEventCharacter.h"

// エンジン
#include "Engine/Common/CustomMap.h"

// 最小エンジンのインクルード
#include "Engine/MiniEngine.h"

// 利用モジュール
#include "EventModule.h"
#include "LevelModule.h"

namespace Lua
{
    struct LuaFuncData;
}

namespace InGame
{
    /// <summary>
    /// ステージに配置するオブジェクト生成(敵/プレイヤー/ボス/建造物...)
    /// ステージのワークフロー制御
    /// </summary>
    class InGameStageManagerComponent final : public Level::LevelBaseComponent
    {
        HE_CLASS_COPY_NG(InGameStageManagerComponent);
        HE_CLASS_MOVE_NG(InGameStageManagerComponent);
        HE_GENERATED_CLASS_BODY_HEADER(InGameStageManagerComponent, Level::LevelBaseComponent);

    public:
        //	コンストラクタ
        InGameStageManagerComponent(
            const Core::Common::Handle& in_rViewHandle,
            const Core::Common::Handle& in_rPlayerParamaterAssetHandle,
            const Core::Common::Handle& in_rEnemyParamterAssetHandle,
            const Core::Common::Handle& in_rStageTimelineParamaterAssetHandle,
            Core::Memory::SharedPtr<Game::GameAssetMap> in_spGameAssetMap);
        virtual ~InGameStageManagerComponent() = default;

        /// <summary>
        /// タスク開始
        /// </summary>
        HE::Bool VBegin() override final;

        /// <summary>
        /// タスク終了
        /// </summary>
        HE::Bool VEnd() override final;

        /// <summary>
        /// 更新
        /// </summary>
        void VUpdate(const HE::Float32) override final;

        /*
                // 初期化、終了
                // 更新、描画で十分
                //	ここでコリジョン判定をする
                virtual bool update(void);

                //	データ通知
                virtual void VOnCallbackSendMessage(const int in_DataType, void* in_pData);
                */

        // void SetViewHandle(const Core::Common::Handle&);

        /// <summary>
        /// リスナーがイベント受け取ったかどうか
        /// </summary>
        // HE::Bool VHandleEvent(Event::EventDataInterfacePtr const& in_rEventData) override final;

        const Core::Common::Handle& GetPlayerHandle() const { return this->_playerHandle; }

    private:
        void _HandleLuaEvent(const Lua::LuaFuncData&);

        HE::Bool _HandleCharacterEvent(Event::EventDataInterfacePtr const&);

        /*
            //	定義
            typedef struct
            {
                char* pEventName;
                void (StageManagerActor::*pFunc)(lua_State* in_pLuaState);
            } _EVENT_FUNC;

            enum
            {
                _STATE_SCROLL = 0,
                _STATE_BOSS,
                _STATE_GAME_END,
            };

            static _EVENT_FUNC m_aEventFuncList[];

            //	オーバーライド関数
            //	登録した自分以外のアクターを取得する。
            virtual void _VOnCallbackOtherActorData(C_PaketSendActor* in_pOtherActor);
            //	スクリプトで通知したデータを受け取る
            virtual void _VOnCallbackScriptEvent(lua_State* in_pLuaState);

    */
        //	関数
        //	クリア
        void _Clear();

        /// <summary>
        /// イベントから敵を生成
        /// </summary>
        void _CreateEnemey(EventCharacterPutEnemy*);

    private:
        Core::Common::Handle _viewHandle;
        Core::Common::Handle _playerParamaterAssetHandle;
        Core::Common::Handle _enemyParamaterAssetHandle;
        Core::Common::Handle _stageTimelineParamaterAssetHandle;
        Core::Common::Handle _playerHandle;
        Core::Common::Handle _characterEventListenerHandle;
        Core::Memory::SharedPtr<Game::GameAssetMap> _spGameAssetMap;

        Core::Common::FixedMap<HE::Uint32, Core::Common::Handle, 256> _mEnemyMap;
        HE::Float32 _fTime      = 0.0f;
        HE::Uint32 _uTimelineNo = 0;
        HE::Uint32 _uEnemyId    = 0;
        HE::Bool _bTimeline     = TRUE;
    };

}  // namespace InGame
