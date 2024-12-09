#pragma once

#include <list>
#include <set>

#include "Engine/Common/CustomMap.h"
#include "Engine/Common/CustomVector.h"
#include "Engine/MiniEngine.h"
#include "EventInterface.h"

namespace Event
{
    using EventListenerList = std::vector<EventListenerPtr>;

    /// <summary>
    /// イベント管理のストラテジーインターフェイス
    /// </summary>
    class EventManagerStrategyInterface
    {
    public:
        EventManagerStrategyInterface()             = default;
        virtual ~EventManagerStrategyInterface()    = default;
        virtual HE::Bool VIsEventTypeHash(const HE::Uint64) = 0;
    };

    /// <summary>
    /// イベント管理
    /// 複数のイベントタイプを扱える
    /// </summary>
    class EventManager final
    {
    public:
        enum EConstants
        {
            EConstants_NumQueues = 2,
            EConstancs_Infinite  = 0xffffffff
        };

    public:
        EventManager(Core::Memory::UniquePtr<EventManagerStrategyInterface>);
        virtual ~EventManager();

        /// <summary>
        /// イベントがないかどうか
        /// </summary>
        HE::Bool EmptyEvent() const;

        // 手動解放
        void Release();

        /// <summary>
        /// リスナー登録
        /// 登録したらTRUEを返す
        /// すでに登録済みなど登録失敗したらFALSE
        /// </summary>
        HE::Bool AddListener(EventListenerPtr const&, EventTypeStr const&);

        HE::Bool RemoveListener(EventListenerPtr const&, EventTypeStr const&);
        HE::Bool RemoveAllListener(EventTypeStr const&);
        /*
                HE::Bool VTrigger(EventDataInterfacePtr const&) const ;
        */

        HE::Bool QueueEvent(EventDataInterfacePtr const&);

#if 0
        HE::Bool VAbortEvent(EventTypeStr const&) override final;
#endif

        HE::Bool Tick(const HE::Uint32);

        HE::Bool ValidateType(EventTypeStr const&) const;
        HE::Bool ValidateHash(const HE::Uint64) const;

        // 情報探索メソッド

        // 特定のイベント型に関連づけられたリスナーのリストを取得
        HE::Bool OutputListenerList(EventListenerList*, EventTypeStr const&) const;

    private:
        // TODO: stdのデータ構造はすべて自前で作成したカスタム用に差し替える予定

        // グローバルインスタンスを一つ定義
        using EventTypeSet = std::set<HE::Uint64>;
        // イベント型のセットに結果を導入
        using EventTypeSetRes = std::pair<EventTypeSet::iterator, HE::Bool>;
        // イベント型ごとに一つのリスト(マップに格納される)
        using EventListenerTable = std::list<EventListenerPtr>;
        // イベント識別子をリスナーリストにマッピング
        using EventListenerMap = Core::Common::FixedMap<HE::Uint64, EventListenerTable, 512>;
        // 処理待ちイベントのキュー、または処理中のイベントのキュー
        using EventQueue = std::list<EventDataInterfacePtr>;

        // イベント型をリスナーにマッピング
        EventListenerMap _mRegistry;

        // 二重バッファのイベント処理のキュー
        EventQueue _aQueue[EConstants_NumQueues];

        // どのキューがアクティブ処理されているか？
        // キューに入ろうとするイベントは他方のキューに置かれる
        HE::Sint32 _iActiveQueue = 0;

        Core::Memory::UniquePtr<EventManagerStrategyInterface> _upStrategy;
    };
}  // namespace Event
