#include "Event/EventManager.h"

#include <chrono>

#include "Engine/Common/Hash.h"
#include "Engine/Common/UUID.h"

namespace Event
{
    EventProcess::EventProcess(const HE::Char* in_szName)
    {
        this->_hash = Core::Common::HashName(in_szName);
    }

    EventProcess::~EventProcess()
    {
        this->Release();
        this->_iActiveQueue = 0;
    }

    HE::Bool EventProcess::EmptyEvent() const
    {
        for (HE::Uint32 i = 0; i < HE_ARRAY_NUM(this->_aQueue); ++i)
        {
            if (0 < this->_aQueue[i].size()) return FALSE;
        }

        return TRUE;
    }

    void EventProcess::Release()
    {
        // 詰んだイベントの解放
        {
            for (auto itr = this->_aQueue->begin(); itr != this->_aQueue->end(); ++itr)
            {
                itr->_Get_rest()._Myfirst._Val.reset();
            }
        }

        // 登録したリスナーを解放
        {
            for (auto itr = this->_mListener.Begin(); itr != this->_mListener.End(); ++itr)
            {
                for (auto itr2 = itr->_data->Begin(); itr2 != itr->_data->End(); ++itr2)
                {
                    itr2->_data.reset();
                    itr2->_data = NULL;
                }
                HE_SAFE_DELETE_MEM(itr->_data);
            }
            this->_mListener.Clear();

            for (auto itr = this->_mListenerBySpecalEvent.Begin();
                 itr != this->_mListenerBySpecalEvent.End(); ++itr)
            {
                for (auto itr2 = itr->_data->Begin(); itr2 != itr->_data->End(); ++itr2)
                {
                    itr2->_data.reset();
                    itr2->_data = NULL;
                }
                HE_SAFE_DELETE_MEM(itr->_data);
            }
            this->_mListenerBySpecalEvent.Clear();
        }
    }

    const Core::Common::Handle EventProcess::AddListener(EventListenerPtr const& in_rListener,
                                                         const HE::Hash in_hash)
    {
        if (in_hash == BaseEventData::ListenerSpecal)
            return this->_AddListener(&this->_mListenerBySpecalEvent, in_rListener, in_hash);

        return this->_AddListener(&this->_mListener, in_rListener, in_hash);
    }

    const Core::Common::Handle EventProcess::AddListener(EventListenerPtr const& in_rListener,
                                                         const HE::Char* in_szName)
    {
        auto hash = Core::Common::HashName(in_szName);
        return this->AddListener(in_rListener, hash);
    }

    HE::Bool EventProcess::RemoveListener(const Core::Common::Handle in_handle)
    {
        if (this->_RemoveListener(&this->_mListener, in_handle)) return TRUE;
        if (this->_RemoveListener(&this->_mListenerBySpecalEvent, in_handle)) return TRUE;

        return FALSE;
    }

    HE::Bool EventProcess::RemoveAllListener()
    {
        this->_mListener.Clear();
        this->_mListenerBySpecalEvent.Clear();

        return TRUE;
    }

    /*
        HE::Bool EventProcess::VTrigger(EventDataInterfacePtr const& in_rEvent) const
        {
            HE::Bool bProc = FALSE;

            auto itrSpecalEvant = this->_mListener.FindKey(BaseEventData::ListenerSpecal);
            if (itrSpecalEvant != this->_mListener.End())
            {
                EventListenerTable const& pTable = itrSpecalEvant->data;
                for (EventListenerTable::const_iterator it2 = pTable.begin(), it2End = pTable.end();
                     it2 != it2End; it2++)
                {
                    if ((*it2)->VHandleEvent(in_rEvent))
                    {
                        bProc = TRUE;
                    }
                }
            }

            if (this->VValidateType(in_rEvent->VNetworkHash()) == FALSE) return bProc;

            auto itr = this->_mListener.FindKey(in_rEvent->VNetworkHash());
            if (itr == this->_mListener.End()) return bProc;

            EventListenerTable const& pTable = itr->data;
            for (EventListenerTable::const_iterator it2 = pTable.begin(), it2End = pTable.end();
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

    HE::Bool EventProcess::QueueEvent(EventDataInterfacePtr const& in_spEvent,
                                      const HE::Hash in_listenerHash)
    {
        HE_ASSERT(0 <= this->_iActiveQueue);
        HE_ASSERT(this->_iActiveQueue < EConstants_NumQueues);

        this->_aQueue[this->_iActiveQueue].push_back(std::tuple(in_listenerHash, in_spEvent));

        return TRUE;
    }

    HE::Bool EventProcess::QueueEvent(EventDataInterfacePtr const& in_spEvent,
                                      const HE::Char* in_szListenerName)
    {
        auto hash = Core::Common::HashName(in_szListenerName);
        return this->QueueEvent(in_spEvent, hash);
    }

#if 0
    HE::Bool EventProcess::VAbortEvent(EventTypeStr const& in_rType)
    {
        HE_ASSERT(0 <= this->_sActiveQueue);
        HE_ASSERT(this->_sActiveQueue < EConstants_NumQueues);

        if (this->VValidateType(in_rType) == FALSE) return FALSE;

        auto it = this->_mListener.FindKey(in_rType.VHash());

        // イベントタイプのハッシュに対応したリスナーがあるかどうか
        if (it == this->_mListener.End()) return FALSE;

        HE::Bool bProcResult     = FALSE;
        EventQueue& evtQueue = this->_aQueue[this->_sActiveQueue];

        for (EventQueue::iterator it = evtQueue.begin(), itEnd = evtQueue.end(); it != itEnd; it++)
        {
            if ((*it)->VEventTypeName() == in_rType)
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

    HE::Bool EventProcess::Tick(const HE::Uint32 in_uMaxMillis)
    {
        // 計測開始時間
        auto startClock = std::chrono::system_clock::now();

        // アクティブなキューを交換する。
        // 交換後の新しいキューは空でなければならない。
        HE::Sint32 iQueueToProcess = this->_iActiveQueue;

        // 新しいキューのイベントはクリア
        this->_iActiveQueue = (this->_iActiveQueue + 1) % EConstants_NumQueues;
        this->_aQueue[this->_iActiveQueue].clear();

        while (0 < this->_aQueue[iQueueToProcess].size())
        {
            // リスナーに投げるイベント取り出す
            auto [listenerHash, spEvent] = this->_aQueue[iQueueToProcess].front();
            this->_aQueue[iQueueToProcess].pop_front();

            // 特殊イベントタイプのリスナー処理
            {
                for (auto it = this->_mListenerBySpecalEvent.Begin();
                     it != this->_mListenerBySpecalEvent.End(); ++it)
                {
                    // 特殊イベントリスナーはイベントタイプに関係なくすべてのイベントを受け取る
                    // 特殊イベントリスナーが登録されていない場合はスキップ
                    if (it->_data->Size() <= 0) continue;
                    // 特殊イベントリスナーを取得
                    auto pTable = it->_data;
                    for (auto listener = pTable->Begin(), end = pTable->End(); listener != end;
                         ++listener)
                    {
                        // 特殊イベントは処理失敗しても継続
                        listener->_data->VHandleEvent(spEvent);
                    }
                }
            }

            // イベントタイプに対応したリスナーにイベントを投げる
            {
                // イベントタイプに対応したリスナーを取得
                auto itListeners = this->_mListener.FindKey(listenerHash);

                // イベントで指定したタイプのリスナーがいないのでスキップ
                if (itListeners == this->_mListener.End()) continue;

                // イベントタイプに対応したリスナーを呼ぶ
                const auto uEventId = itListeners->_key;
                auto pTable         = itListeners->_data;

                for (auto listener = pTable->Begin(), end = pTable->End(); listener != end;
                     ++listener)
                {
                    // リスナーがイベント処理に失敗したら以降のリスナーはイベント処理しない
                    if (listener->_data->VHandleEvent(spEvent)) break;
                }

                // 指定時間を超えた場合はイベント処理を中断
                if (in_uMaxMillis != EventProcess::EConstancs_Infinite)
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
                auto [hash, spEvent] = this->_aQueue[iQueueToProcess].back();
                this->_aQueue[iQueueToProcess].pop_back();
                this->_aQueue[this->_iActiveQueue].push_front(std::tuple(hash, spEvent));
            }
        }

        // キュー内にイベントがないか
        return bQueueFlushed;
    }

    /*
        HE::Bool EventProcess::ValidateType(EventTypeStr const& in_rType) const
        {
            if (in_rType.Size() <= 0) return FALSE;

            auto ulHash = in_rType.Hash();
            if ((in_rType.Hash() == 0) && (HE_STR_CMP(in_rType.Str(), HE_STR_TEXT("*")) != 0))
                return FALSE;

            return this->ValidateHash(ulHash);
        }

        HE::Bool EventProcess::ValidateHash(const HE::Hash in_hash) const
        {
            return (this->_hash == in_hash);
            // return this->_upStrategy->VIsHash(in_hash);
        }
    */
    // 情報探索メソッド

    // 特定のイベント型に関連づけられたリスナーのリストを取得
    HE::Bool EventProcess::OutputListenerList(EventListenerList* out,
                                              const HE::Char* in_szListenerTypeName) const
    {
        auto hash = Core::Common::HashName(in_szListenerTypeName);
        return this->OutputListenerList(out, hash);
    }

    HE::Bool EventProcess::OutputListenerList(EventListenerList* out, const HE::Hash in_hash) const
    {
        HE_ASSERT(out);

        auto itListeners = this->_mListener.FindKey(in_hash);

        // イベント型には現在リスナーがない
        if (itListeners == this->_mListener.End()) return FALSE;

        auto pTable = itListeners->_data;

        // このイベント型には以前はリスナーがあったが今はない
        if (pTable->Size() <= 0) return FALSE;

        out->reserve(pTable->Size());
        out->clear();

        for (auto it = pTable->Begin(), end = pTable->End(); it != end; ++it)
        {
            out->push_back(it->_data);
        }

        return TRUE;
    }

    const Core::Common::Handle EventProcess::_AddListener(EventListenerMap* in_mpListener,
                                                          EventListenerPtr const& in_rListener,
                                                          const HE::Hash in_hash)
    {
        // リスナーマップのエントリ探し、
        // エントリに対応するテーブルがなければエントリ作成
        auto itr = in_mpListener->FindKey(in_hash);

        // リスナーに登録されていない場合は登録する
        if (itr == in_mpListener->End())
        {
            auto pTable = HE_NEW_MEM(EventListenerTable, 0)();
            itr         = in_mpListener->Add(in_hash, pTable);
            if (itr.IsValid() == FALSE) return NullHandle;
        }

        // リスナーマップリスト更新

        // mapテーブルからハッシュ指定したテーブルデータリストを取得
        EventListenerTable& rTable = *(itr->_data);

        // マップから取得した要素にあるリスナーリストにすでに登録されているかチェック
        // すでに登録済みならリスト登録はスキップ
        Core::Common::Handle handle = Core::Common::GenerateUUIDBy64Bit();
        HE_ASSERT_RETURN_VALUE(NullHandle, (rTable.Contains(handle) == FALSE));
        // イベント型の有効性を確認
        // イベントリスナーの有効性を確認
        // イベントリスナーはまだマップにないから追加
        rTable.Add(handle, in_rListener);

        // 登録に成功
        return handle;
    }

    HE::Bool EventProcess::_RemoveListener(EventListenerMap* in_mpListener,
                                           const Core::Common::Handle in_handle)
    {
        HE::Bool bErase = FALSE;
        for (auto itr = in_mpListener->Begin(); itr != in_mpListener->End(); ++itr)
        {
            auto pTable = itr->_data;
            for (auto it2 = pTable->Begin(), it2End = pTable->End(); it2 != it2End; ++it2)
            {
                if (it2->_key == in_handle)
                {
                    // 目的のリスナーを発見。テーブルから削除
                    pTable->Erase(it2);

                    // リターンコードを更新
                    bErase = TRUE;

                    // VAddListenr()でリスナーの重複がないようにしているから
                    // これ以上の検索は不要

                    break;
                }
            }
        }
        return bErase;
    }

}  // namespace Event
