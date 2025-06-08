#pragma once

#include "Event/EventManager.h"

namespace EventTest
{
    static const HE::Char* s_szTestEventType  = HE_STR_TEXT("TestEvent");
    static const HE::Char* s_szTestEventType2 = HE_STR_TEXT("TestEvent2");

    class EvtDataTextPut final : public Event::BaseEventData
    {
    public:
        EvtDataTextPut(const Event::EventTypeStr& in_rEventType)
            : Event::BaseEventData(in_rEventType), _sCount(0)
        {
        }

    public:
        inline static Event::EventTypeStr _szDataType = HE_STR_TEXT("test");

        HE::Sint32 _sCount = 0;
    };
}  // namespace EventTest

TEST_CASE("Event System")
{
    class TestListener final : public Event::EventListenerInterface
    {
    public:
        explicit TestListener() {}

        const HE::Char* VName() override final { return HE_STR_TEXT("C_TestListener"); }

        HE::Bool VHandleEvent(Event::EventDataInterfacePtr const& in_spEvent) override final
        {
            if (EventTest::EvtDataTextPut::_szDataType.Hash() == in_spEvent->VEventHash())
            {
                EventTest::EvtDataTextPut* pEvtData =
                    reinterpret_cast<EventTest::EvtDataTextPut*>(in_spEvent.get());
                HE_ASSERT(pEvtData);

                pEvtData->_sCount += 1;

                HE_LOG_LINE(HE_STR_TEXT("test [%d]"), pEvtData->_sCount);
            }

            return TRUE;
        }
    };

    // カスタムヒープを使うのでメモリアロケーターを作成
    Core::Memory::Manager memoryManager;
    CHECK(memoryManager.Start(0x1000000));

    // ページ確保テスト
    {
        // メモリサイズのイニシャライズ
        Core::Memory::Manager::PageSetupInfo memoryPageSetupInfoArray[] = {
            // 複数ページのサイズ
            {0, 3 * 1024 * 1024}, {1, 4 * 1024 * 1024}, {2, 2 * 1024 * 1024},
            {3, 2 * 1024 * 1024}, {4, 2 * 1024 * 1024}, {5, 3 * 1024 * 1024},
        };

        CHECK(memoryManager.SetupMemoryPage(memoryPageSetupInfoArray,
                                            HE_ARRAY_NUM(memoryPageSetupInfoArray)));
        CHECK(memoryManager.CheckAllMemoryBlock());
    }

    Event::EventProcess eventProcess(HE_STR_TEXT("Test"));

    HE_LOG_LINE(HE_STR_TEXT("EventMangerTest"));

    // リスナー登録
    auto spTestListenr = HE_MAKE_CUSTOM_UNIQUE_PTR((TestListener));

    const HE::Char* szListenerType01 = HE_STR_TEXT("Listener01");

    // リスナー追加は初回なので必ず成功する
    CHECK(eventProcess.AddListener(std::move(spTestListenr), szListenerType01));

    // イベントは生成して所有権は管理側に渡す
    auto spTestEvent =
        HE_MAKE_CUSTOM_UNIQUE_PTR((EventTest::EvtDataTextPut), EventTest::s_szTestEventType);
    // 指定したリスナーへイベント送る
    CHECK(eventProcess.QueueEvent(std::move(spTestEvent), szListenerType01));

    // 登録しているイベント型名から登録リスナーを出力
    // 共有ポインタなので取得したリストは解放しないとメモリが残る
    {
        Event::EventListenerList typeList;

        CHECK(eventProcess.OutputListenerList(&typeList, szListenerType01));
        for (Event::EventListenerList::const_iterator i = typeList.begin(); i != typeList.end();
             i++)
        {
            HE_LOG_LINE(HE_STR_TEXT("%s"), (*i)->VName());
        }
    }

    while (eventProcess.EmptyEvent() == FALSE)
    {
        CHECK(eventProcess.Tick(Event::EventProcess::EConstancs_Infinite));
    }

    // 戦略アリゴリズムで確保したヒープを解放してメモリアロケーター破棄でエラーにならないようにする
    eventProcess.Release();

    CHECK(memoryManager.VRelease());
    memoryManager.Reset();
}

TEST_CASE("Event System All Remove Listener")
{
    class TestListener final : public Event::EventListenerInterface
    {
    public:
        explicit TestListener() {}

        const HE::Char* VName() override final { return HE_STR_TEXT("C_TestListener"); }

        HE::Bool VHandleEvent(Event::EventDataInterfacePtr const&) override final { return TRUE; }
    };

    // カスタムヒープを使うのでメモリアロケーターを作成
    Core::Memory::Manager memoryManager;
    CHECK(memoryManager.Start(0x1000000));

    // ページ確保テスト
    {
        // メモリサイズのイニシャライズ
        Core::Memory::Manager::PageSetupInfo memoryPageSetupInfoArray[] = {
            // 複数ページのサイズ
            {0, 3 * 1024 * 1024}, {1, 4 * 1024 * 1024}, {2, 2 * 1024 * 1024},
            {3, 2 * 1024 * 1024}, {4, 2 * 1024 * 1024}, {5, 3 * 1024 * 1024},
        };

        CHECK(memoryManager.SetupMemoryPage(memoryPageSetupInfoArray,
                                            HE_ARRAY_NUM(memoryPageSetupInfoArray)));
        CHECK(memoryManager.CheckAllMemoryBlock());
    }

    Event::EventProcess eventProcess(HE_STR_TEXT("Test02"));

    HE_LOG_LINE(HE_STR_TEXT("EventMangerTest RemoveAllListener"));

    // リスナー登録
    // イベントタイプが違うリスナーを登録
    Core::Common::Handle handle01;
    Core::Common::Handle handle02;
    {
        auto spTestListenr               = HE_MAKE_CUSTOM_UNIQUE_PTR((TestListener));
        auto spTestListenr2              = HE_MAKE_CUSTOM_UNIQUE_PTR((TestListener));
        const HE::Char* szListenerType01 = HE_STR_TEXT("ListenerType01");
        const HE::Char* szListenerType02 = HE_STR_TEXT("ListenerType02");

        // リスナー追加は初回なので必ず成功する
        handle01 = eventProcess.AddListener(std::move(spTestListenr), szListenerType01);
        CHECK(handle01.Null() == FALSE);
        handle02 = eventProcess.AddListener(std::move(spTestListenr2), szListenerType02);
        CHECK(handle01.Null() == FALSE);
    }

    // 指定タイプのリスナーを一括破棄
    CHECK(eventProcess.RemoveListener(handle01));
    CHECK(eventProcess.RemoveListener(handle02));

    // 戦略アリゴリズムで確保したヒープを解放してメモリアロケーター破棄でエラーにならないようにする
    eventProcess.Release();

    CHECK(memoryManager.VRelease());
    memoryManager.Reset();
}
