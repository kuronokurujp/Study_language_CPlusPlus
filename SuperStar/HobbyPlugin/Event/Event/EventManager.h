#pragma once

#include <list>
#include <set>

#include "Engine/Common/CustomMap.h"
#include "Engine/Common/CustomVector.h"
#include "Engine/Common/PoolManager.h"
#include "Engine/MiniEngine.h"
#include "EventInterface.h"

namespace Event
{
    using EventListenerList = std::vector<EventListenerPtr>;

    /*
        /// <summary>
        /// イベントプロセスのストラテジーインターフェイス
        /// </summary>
        class EventProcessStrategyInterface
        {
        public:
            EventProcessStrategyInterface()          = default;
            virtual ~EventProcessStrategyInterface() = default;
            virtual HE::Bool VIsHash(const HE::Hash) = 0;
            // TODO: ハッシュ値
            virtual HE::Hash VHash() const = 0;
        };
        */

    /// <summary>
    /// イベントプロセス
    /// </summary>
    class EventProcess final
    {
    public:
        enum EConstants
        {
            EConstants_NumQueues = 2,
            EConstancs_Infinite  = 0xffffffff
        };

    public:
        // EventProcess(Core::Memory::UniquePtr<EventProcessStrategyInterface>);
        EventProcess(const HE::Char* in_szName);
        virtual ~EventProcess();

        /// <summary>
        /// イベントがないかどうか
        /// </summary>
        HE::Bool EmptyEvent() const;

        // 手動解放
        void Release();

        /// <summary>
        /// リスナー登録
        /// 登録したらリスナーのハッシュ値を返す
        /// 失敗したら0
        /// </summary>
        const Core::Common::Handle AddListener(EventListenerPtr const&, const HE::Hash);

        HE::Bool RemoveListener(const HE::Uint64);
        HE::Bool RemoveAllListener();
        /*
                HE::Bool VTrigger(EventDataInterfacePtr const&) const ;
        */

        HE::Bool QueueEvent(EventDataInterfacePtr const&, const HE::Hash);

#if 0
        HE::Bool VAbortEvent(EventTypeStr const&) override final;
#endif

        HE::Bool Tick(const HE::Uint32);

        // HE::Bool ValidateType(EventTypeStr const&) const;
        // HE::Bool ValidateHash(const HE::Hash) const;

        // 情報探索メソッド

        // 特定のイベント型に関連づけられたリスナーのリストを取得
        HE::Bool OutputListenerList(EventListenerList*, const HE::Hash) const;

    private:
        // TODO: stdのデータ構造はすべて自前で作成したカスタム用に差し替える予定

        // グローバルインスタンスを一つ定義
        using EventTypeSet = std::set<HE::Uint64>;
        // イベント型のセットに結果を導入
        using EventTypeSetRes = std::pair<EventTypeSet::iterator, HE::Bool>;
        // イベント型ごとに一つのリスト(マップに格納される)
        // using EventListenerTable = std::list<EventListenerPtr>;
        using EventListenerTable =
            Core::Common::FixedMap<Core::Common::Handle, EventListenerPtr, 128>;
        // イベント識別子をリスナーリストにマッピング
        using EventListenerMap = Core::Common::FixedMap<HE::Uint64, EventListenerTable*, 512>;
        // 処理待ちイベントのキュー、または処理中のイベントのキュー
        using EventQueue = std::list<std::tuple<HE::Hash, EventDataInterfacePtr>>;

    private:
        const Core::Common::Handle _AddListener(EventListenerMap* in_mpListener,
                                                EventListenerPtr const& in_rListener,
                                                const HE::Hash in_hash);

        HE::Bool _RemoveListener(EventListenerMap*, const HE::Uint64);

    private:
        // イベント型をリスナーにマッピング
        EventListenerMap _mListener;
        EventListenerMap _mListenerBySpecalEvent;

        // 二重バッファのイベント処理のキュー
        EventQueue _aQueue[EConstants_NumQueues];

        // どのキューがアクティブ処理されているか？
        // キューに入ろうとするイベントは他方のキューに置かれる
        HE::Sint32 _iActiveQueue = 0;

        HE::Hash _hash = 0;

        // Core::Memory::UniquePtr<EventProcessStrategyInterface> _upStrategy;
    };
}  // namespace Event
