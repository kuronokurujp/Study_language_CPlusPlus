#include "EventManager.h"

#include <chrono>

#include "Engine/Common/Hash.h"

namespace Event
{
    EventManager::EventManager(Core::Memory::UniquePtr<EventNetworkStrategyInterface> in_upStrategy)
        : _iActiveQueue(0)
    {
        this->_upStrategy = std::move(in_upStrategy);
    }

    EventManager::~EventManager()
    {
        this->Release();
        this->_iActiveQueue = 0;
    }

    HE::Bool EventManager::EmptyEvent() const
    {
        for (HE::Uint32 i = 0; i < HE_ARRAY_NUM(this->_aQueue); ++i)
        {
            if (0 < this->_aQueue[i].size()) return FALSE;
        }

        return TRUE;
    }

    void EventManager::Release()
    {
        // 詰んだイベントの解放
        {
            for (auto itr = this->_aQueue->begin(); itr != this->_aQueue->end(); ++itr)
            {
                itr->reset();
            }
        }

        // 登録したリスナーを解放
        {
            for (auto itr = this->_mRegistry.Begin(); itr != this->_mRegistry.End(); ++itr)
            {
                for (auto itr2 = itr->_data.begin(); itr2 != itr->_data.end(); ++itr2)
                {
                    itr2->reset();
                    (*itr2) = NULL;
                }

                itr->_data.clear();
            }
            this->_mRegistry.Clear();
        }

        // 拡張ストラテジーを解放
        HE_SAFE_DELETE_UNIQUE_PTR(this->_upStrategy);
    }

    const HE::Uint64 EventManager::AddListener(EventListenerPtr const& in_rListener,
                                               EventTypeStr const& in_rType)
    {
        if (this->ValidateType(in_rType) == FALSE) return 0;

        // リスナーマップのエントリ探し、
        // エントリに対応するテーブルがなければエントリ作成
        auto itr = this->_mRegistry.FindKey(in_rType.Hash());

        // リスナーに登録されていない場合は登録する
        if (itr == this->_mRegistry.End())
        {
            itr = this->_mRegistry.Add(in_rType.Hash(), EventListenerTable());
            if (itr.IsValid() == FALSE) return 0;
        }

        // リスナーマップリスト更新

        // mapテーブルからハッシュ指定したテーブルデータリストを取得
        EventListenerTable& rTable = itr->_data;

        // マップから取得した要素にあるリスナーリストにすでに登録されているかチェック
        // すでに登録済みならリスト登録はスキップ
        auto ulHash = Core::Common::HashName(in_rListener->VName());
        for (EventListenerTable::iterator it = rTable.begin(), itEnd = rTable.end(); it != itEnd;
             it++)
        {
            // TODO: 同じハッシュ値のリスナーがあったら失敗にする
            auto ulCheckHash = Core::Common::HashName((*it)->VName());
            HE_ASSERT_RETURN_VALUE(ulCheckHash != ulHash, 0);
        }

        // イベント型の有効性を確認
        // イベントリスナーの有効性を確認
        // イベントリスナーはまだマップにないから追加
        rTable.push_back(in_rListener);

        // 登録に成功
        return ulHash;
    }

    HE::Bool EventManager::RemoveListener(const HE::Uint64 in_ulListenrHash,
                                          EventTypeStr const& in_rType)
    {
        if (this->ValidateType(in_rType) == FALSE) return FALSE;
        HE::Bool bErase = FALSE;

        // 総当りの手法 合致するリスナーが見つかるまで、
        // 既存のマッピングのエントリをたどる、見つかったら削除する
        for (auto itr = this->_mRegistry.Begin(); itr != this->_mRegistry.End(); ++itr)
        {
            EventListenerTable& table = itr->_data;
            for (EventListenerTable::iterator it2 = table.begin(), it2End = table.end();
                 it2 != it2End; it2++)
            {
                auto checkHash = Core::Common::HashName((*it2)->VName());
                if (checkHash == in_ulListenrHash)
                {
                    // 目的のリスナーを発見。テーブルから削除
                    table.erase(it2);

                    // リターンコードを更新
                    bErase = TRUE;

                    // VAddListenr()でリスナーの重複がないようにしているから
                    // これ以上の検索は不要

                    break;
                }
            }
        }

        // TODO: リスナーが一つもないならリスナー枠を削除

        return bErase;
    }

    HE::Bool EventManager::RemoveAllListener(EventTypeStr const& in_rType)
    {
        if (this->ValidateType(in_rType) == FALSE) return FALSE;

        auto tableItr = this->_mRegistry.FindKey(in_rType.Hash());
        if (tableItr == this->_mRegistry.End()) return FALSE;

        // 共有ポインターを完全破棄
        for (auto itr = tableItr->_data.begin(); itr != tableItr->_data.end(); ++itr)
        {
            itr->reset();
            (*itr) = NULL;
        }

        // イベントタイプのリスナーを丸ごと破棄
        this->_mRegistry.Erase(in_rType.Hash());

        return TRUE;
    }

    /*
        HE::Bool EventManager::VTrigger(EventDataInterfacePtr const& in_rEvent) const
        {
            HE::Bool bProc = FALSE;

            auto itrSpecalEvant = this->_mRegistry.FindKey(BaseEventData::EType_SpecalEvent);
            if (itrSpecalEvant != this->_mRegistry.End())
            {
                EventListenerTable const& table = itrSpecalEvant->data;
                for (EventListenerTable::const_iterator it2 = table.begin(), it2End = table.end();
                     it2 != it2End; it2++)
                {
                    if ((*it2)->VHandleEvent(in_rEvent))
                    {
                        bProc = TRUE;
                    }
                }
            }

            if (this->VValidateType(in_rEvent->VNetworkHash()) == FALSE) return bProc;

            auto itr = this->_mRegistry.FindKey(in_rEvent->VNetworkHash());
            if (itr == this->_mRegistry.End()) return bProc;

            EventListenerTable const& table = itr->data;
            for (EventListenerTable::const_iterator it2 = table.begin(), it2End = table.end();
                 it2 != it2End; it2++)
            {
                EventListenerPtr listener = *it2;
                if (listener->VHandleEvent(in_rEvent))
                {
                    // メッセージを受け入れる場合は trueにする
                    bProc = TRUE;
                }
            }

            // イベント処理が一つでも成功したか
            return bProc;
        }
    */

    HE::Bool EventManager::QueueEvent(EventDataInterfacePtr const& in_spEvent)
    {
        HE_ASSERT(0 <= this->_iActiveQueue);
        HE_ASSERT(this->_iActiveQueue < EConstants_NumQueues);

        if (this->ValidateHash(in_spEvent->VNetworkHash()) == FALSE) return FALSE;

        this->_aQueue[this->_iActiveQueue].push_back(in_spEvent);

        return TRUE;
    }
#if 0
    HE::Bool EventManager::VAbortEvent(EventTypeStr const& in_rType)
    {
        HE_ASSERT(0 <= this->_sActiveQueue);
        HE_ASSERT(this->_sActiveQueue < EConstants_NumQueues);

        if (this->VValidateType(in_rType) == FALSE) return FALSE;

        auto it = this->_mRegistry.FindKey(in_rType.Hash());

        // イベントタイプのハッシュに対応したリスナーがあるかどうか
        if (it == this->_mRegistry.End()) return FALSE;

        HE::Bool bProcResult     = FALSE;
        EventQueue& evtQueue = this->_aQueue[this->_sActiveQueue];

        for (EventQueue::iterator it = evtQueue.begin(), itEnd = evtQueue.end(); it != itEnd; it++)
        {
            if ((*it)->VEventTypeStr() == in_rType)
            {
                it          = evtQueue.erase(it);
                bProcResult = TRUE;
            }
            else
            {
                ++it;
            }
        }

        return bProcResult;
    }
#endif

    HE::Bool EventManager::Tick(const HE::Uint32 in_uMaxMillis)
    {
        // 計測開始時間
        auto startClock = std::chrono::system_clock::now();

        // アクティブなキューを交換する。
        // 交換後の新しいキューは空でなければならない。
        HE::Sint32 iQueueToProcess = this->_iActiveQueue;

        // 新しいキューのイベントはクリア
        this->_iActiveQueue = (this->_iActiveQueue + 1) % EConstants_NumQueues;
        this->_aQueue[this->_iActiveQueue].clear();

        // 特殊イベントをリスナーが受け取り処理
        auto itSpecalListener = this->_mRegistry.FindKey(BaseEventData::EType_SpecalEvent);

        while (0 < this->_aQueue[iQueueToProcess].size())
        {
            // リスナーに投げるイベント取り出す
            EventDataInterfacePtr spEvent = this->_aQueue[iQueueToProcess].front();
            this->_aQueue[iQueueToProcess].pop_front();

            // 特殊イベントタイプのリスナー処理
            {
                if (itSpecalListener != this->_mRegistry.End())
                {
                    EventListenerTable const& table = itSpecalListener->_data;
                    for (EventListenerTable::const_iterator listener         = table.begin(),
                                                            listenerEndPoint = table.end();
                         listener != listenerEndPoint; listener++)
                    {
                        // 特殊イベントは処理失敗しても継続
                        (*listener)->VHandleEvent(spEvent);
                    }
                }
            }

            // イベントタイプに対応したリスナーにイベントを投げる
            {
                // イベントタイプに対応したリスナーを取得
                auto itListeners = this->_mRegistry.FindKey(spEvent->VNetworkHash());

                // イベントで指定したタイプのリスナーがいないのでスキップ
                if (itListeners == this->_mRegistry.End()) continue;

                // イベントタイプに対応したリスナーを呼ぶ
                const HE::Uint32 uEventId        = itListeners->_key;
                EventListenerTable const& lTable = itListeners->_data;

                for (EventListenerTable::const_iterator listener = lTable.begin(),
                                                        end      = lTable.end();
                     listener != end; ++listener)
                {
                    // リスナーがイベント処理に失敗したら以降のリスナーはイベント処理しない
                    if ((*listener)->VHandleEvent(spEvent)) break;
                }

                // 指定時間を超えた場合はイベント処理を中断
                if (in_uMaxMillis != EventManager::EConstancs_Infinite)
                {
                    auto endClock = std::chrono::system_clock::now();

                    // 指定時間を超えたらイベント処理を中断
                    // 処理に要した時間をミリ秒に変換
                    const HE::Uint32 uElapsed =
                        std::chrono::duration_cast<std::chrono::milliseconds>(endClock - startClock)
                            .count();
                    if (in_uMaxMillis <= uElapsed) break;
                }
            }
        }

        const HE::Bool bQueueFlushed = (this->_aQueue[iQueueToProcess].size() == 0);
        {
            // キュー内にイベントが残っている場合は次のフレームでイベントを処理するようにアクティブリストに入れる
            // TODO: 数が多くなると負荷が高い
            while (0 < this->_aQueue[iQueueToProcess].size())
            {
                EventDataInterfacePtr spEvent = this->_aQueue[iQueueToProcess].back();
                this->_aQueue[iQueueToProcess].pop_back();
                this->_aQueue[this->_iActiveQueue].push_front(spEvent);
            }
        }

        // キュー内にイベントがないか
        return bQueueFlushed;
    }

    HE::Bool EventManager::ValidateType(EventTypeStr const& in_rType) const
    {
        if (in_rType.Size() <= 0) return FALSE;

        auto ulHash = in_rType.Hash();
        if ((in_rType.Hash() == 0) && (HE_STR_CMP(in_rType.Str(), HE_STR_TEXT("*")) != 0))
            return FALSE;

        return this->ValidateHash(ulHash);
    }

    HE::Bool EventManager::ValidateHash(const HE::Uint64 in_ulHash) const
    {
        return this->_upStrategy->VIsHash(in_ulHash);
    }

    // 情報探索メソッド

    // 特定のイベント型に関連づけられたリスナーのリストを取得
    HE::Bool EventManager::OutputListenerList(EventListenerList* out,
                                              EventTypeStr const& in_rEventType) const
    {
        HE_ASSERT(out);

        // 無効なイベント型
        if (this->ValidateType(in_rEventType) == FALSE) return FALSE;

        auto itListeners = this->_mRegistry.FindKey(in_rEventType.Hash());

        // イベント型には現在リスナーがない
        if (itListeners == this->_mRegistry.End()) return FALSE;

        EventListenerTable const& table = itListeners->_data;

        // このイベント型には以前はリスナーがあったが今はない
        if (table.size() <= 0) return FALSE;

        out->reserve(table.size());
        out->clear();

        for (EventListenerTable::const_iterator it = table.begin(), end = table.end(); it != end;
             it++)
        {
            out->push_back(*it);
        }

        return TRUE;
    }

}  // namespace Event
