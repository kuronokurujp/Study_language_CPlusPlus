#include <gtest/gtest.h>
#include <time.h>

#include "Engine/Common/CustomMap.h"
#include "Engine/Common/CustomString.h"
#include "Engine/Memory/Memory.h"

namespace Core
{
    // 標準のメモリ確保を使用する<文字列, 整数>のマップ
    typedef Core::Common::FixedMap<Core::Common::FixedString16, HE::Sint32, 2048> BASICMAP;
    std::ostream& operator<<(std::ostream& out, const BASICMAP::Iterator& r)
    {
        (void)r;
        return out;
    }

    TEST(HobbyEngine_Common, FixMapTest)
    {
        Core::Memory::Manager memoryManager;
        EXPECT_EQ(memoryManager.Start(0x1000000), TRUE);
        // ページ確保テスト
        {
            // メモリサイズのイニシャライズ
            Core::Memory::Manager::PageSetupInfo memoryPageSetupInfoArray[] = {
                // 複数ページのサイズ
                {0, 3 * 1024 * 1024}};

            EXPECT_EQ(memoryManager.SetupMemoryPage(memoryPageSetupInfoArray,
                                                    HE_ARRAY_NUM(memoryPageSetupInfoArray)),
                      TRUE);
            EXPECT_EQ(memoryManager.CheckAllMemoryBlock(), TRUE);
        }

        clock_t ctime;
        Core::Common::FixedString16 strKey;

        // データ個数
        const HE::Sint32 ARRAY_NUM = 1024;

        // 乱数を時間で初期化
        srand((unsigned int)time(NULL));

        // データを用意しておく
        HE::Sint32* pDataArray = reinterpret_cast<HE::Sint32*>(
            HE_ALLOC_MEM(sizeof(HE::Sint32) * ARRAY_NUM,
                         0));  // HE_NEW_MEM_ARRAY(Sint32, ARRAY_NUM, 0);
        for (HE::Sint32 ii = 0; ii < ARRAY_NUM; ii++)
        {
            pDataArray[ii] = (HE::Sint32)ii;
        }

        for (HE::Sint32 shuffle = 0; shuffle < 100000; ++shuffle)
        {
            HE::Sint32 a = rand() % ARRAY_NUM;
            HE::Sint32 b = rand() % ARRAY_NUM;

            // 適当に混ぜっ返す
            HE::Sint32 tmp = pDataArray[a];
            pDataArray[a]  = pDataArray[b];
            pDataArray[b]  = tmp;
        }

        // マップの生成と空かどうかのテスト
        BASICMAP testmap;
        EXPECT_EQ(testmap.Empty(), TRUE);

        // データの追加
        {
            ctime = clock();
            for (HE::Sint32 ii = 0; ii < ARRAY_NUM; ii++)
            {
                // データの中身を16進数にしてキーにする
                strKey.Format(HE_STR_TEXT("0x%04x"), pDataArray[ii]);
                // キーとデータを追加する
                testmap.Add(strKey.Str(), pDataArray[ii]);
            }
            // 時間の報告
            HE_LOG_LINE(HE_STR_TEXT("add ctime=%f sec"),
                        (double)(clock() - ctime) / CLOCKS_PER_SEC);
        }

        // 空じゃなくなったかチェック
        EXPECT_EQ(!testmap.Empty(), TRUE);

        // ノードの正当性チェック
        {
            ctime            = clock();
            HE::Bool bResult = testmap.CheckValidByDebug(ARRAY_NUM);
            EXPECT_EQ(bResult, TRUE);
            HE_LOG_LINE(HE_STR_TEXT("check ctime=%f sec"),
                        (double)(clock() - ctime) / CLOCKS_PER_SEC);
        }

        // 検索
        {
            ctime = clock();
            for (HE::Sint32 ii = 0; ii < ARRAY_NUM; ii++)
            {
                // キーを作る
                strKey.Format(HE_STR_TEXT("0x%04x"), ii);

                // 検索
                BASICMAP::Iterator it = testmap.FindKey(strKey.Str());
                EXPECT_NE(it, testmap.End());
                EXPECT_EQ(it->_key, strKey);
                EXPECT_EQ(it->_data, ii);
            }
            HE_LOG_LINE(HE_STR_TEXT("find ctime=%f sec"),
                        (double)(clock() - ctime) / CLOCKS_PER_SEC);
        }

        // イテレータテスト
        {
            // イテレータ用のチェックリスト配列を作る
            HE::Bool* pCheckArray =
                reinterpret_cast<HE::Bool*>(HE_ALLOC_MEM(sizeof(HE::Bool) * ARRAY_NUM, 0));
            HE::Uint32 checked_count = 0;
            for (HE::Sint32 ii = 0; ii < ARRAY_NUM; ii++)
            {
                pCheckArray[ii] = FALSE;
            }

            ctime = clock();
            for (BASICMAP::Iterator it = testmap.Begin(); it != testmap.End(); ++it)
            {
                EXPECT_LE(0, it->_data);
                EXPECT_LT(it->_data, ARRAY_NUM);
                EXPECT_EQ(pCheckArray[it->_data], FALSE);

                // チェック済み
                pCheckArray[it->_data] = TRUE;
                // チェック済みカウント
                checked_count++;
            }

            // イテレータの数はノードの数と一致しているか？
            EXPECT_EQ(checked_count, ARRAY_NUM);
            HE_LOG_LINE(HE_STR_TEXT("iterator ctime=%f sec"),
                        (double)(clock() - ctime) / CLOCKS_PER_SEC);

            HE_SAFE_DELETE_MEM(pCheckArray);
        }

        // 削除
        {
            ctime = clock();
            for (HE::Sint32 ii = 0; ii < ARRAY_NUM; ii++)
            {
                // キーを作って
                strKey.Format(HE_STR_TEXT("0x%04x"), ii);
                // 探す
                BASICMAP::Iterator it = testmap.FindKey(strKey.Str());
                HE::Bool bResult;
                // 削除する
                bResult = testmap.Erase(it);
                EXPECT_EQ(bResult, TRUE);
            }

            HE_LOG_LINE(HE_STR_TEXT("iterator ctime=%f sec"),
                        (double)(clock() - ctime) / CLOCKS_PER_SEC);
        }

        // 全て削除が終わったので、空かどうかチェック
        EXPECT_EQ(testmap.Empty(), TRUE);

        // 添え字でアクセスする
        testmap["one"]   = 1;
        testmap["two"]   = 2;
        testmap["three"] = 3;
        EXPECT_EQ(testmap["one"], 1);
        EXPECT_EQ(testmap["two"], 2);
        EXPECT_EQ(testmap["three"], 3);
        EXPECT_EQ(testmap.Size(), 3);

        // 添え字で書き換え
        testmap["one"]   = 10;
        testmap["two"]   = 20;
        testmap["three"] = 30;
        EXPECT_EQ(testmap["one"], 10);
        EXPECT_EQ(testmap["two"], 20);
        EXPECT_EQ(testmap["three"], 30);
        // 数は変わっていないはず
        EXPECT_EQ(testmap.Size(), 3);

        // クリア関数のテスト
        testmap.Clear();
        EXPECT_EQ(testmap.Empty(), TRUE);

        // データの削除
        HE_SAFE_DELETE_MEM(pDataArray);

        EXPECT_EQ(memoryManager.VRelease(), TRUE);
        memoryManager.Reset();
    }

    TEST(HobbyEngine_Common, FixMapCopy)
    {
        BASICMAP srcmap;

        struct TEST_DATA
        {
            const HE::Char* pKey = 0;
            HE::Sint32 num       = 0;
        };
        static const TEST_DATA s_aArray[] = {
            {HE_STR_TEXT("test1"), 10}, {HE_STR_TEXT("test2"), 20}, {HE_STR_TEXT("test3"), 30},
            {HE_STR_TEXT("test4"), 40}, {HE_STR_TEXT("test5"), 50}, {HE_STR_TEXT("test6"), 60},
        };

        for (HE::Uint32 i = 0; i < HE_ARRAY_NUM(s_aArray); ++i)
        {
            srcmap.Add(s_aArray[i].pKey, s_aArray[i].num);
        }

        BASICMAP dstmap = srcmap;
        EXPECT_EQ(dstmap.Size(), HE_ARRAY_NUM(s_aArray));

        for (HE::Uint32 i = 0; i < HE_ARRAY_NUM(s_aArray); ++i)
        {
            auto iter = dstmap.FindKey(s_aArray[i].pKey);
            EXPECT_EQ(iter.IsValid(), TRUE);
            EXPECT_EQ(iter->_data, s_aArray[i].num);

            HE_LOG_LINE(HE_STR_TEXT("key(%s) / data(%d)"), iter->_key.Str(), iter->_data);
        }
    }

    TEST(HobbyEngine_Common, FixMapErase)
    {
        BASICMAP srcmap;
        srcmap.Add(Core::Common::FixedString16(HE_STR_TEXT("a")), 1);
        srcmap.Add(Core::Common::FixedString16(HE_STR_TEXT("b")), 2);
        srcmap.Add(Core::Common::FixedString16(HE_STR_TEXT("c")), 3);
        srcmap.Add(Core::Common::FixedString16(HE_STR_TEXT("d")), 4);
        srcmap.Add(Core::Common::FixedString16(HE_STR_TEXT("e")), 5);
        srcmap.Add(Core::Common::FixedString16(HE_STR_TEXT("f")), 6);

        // ループ中に要素削除しても大丈夫か
        for (auto itr = srcmap.Begin(); itr != srcmap.End(); ++itr)
        {
            HE_LOG_LINE(HE_STR_TEXT("key(%s), data(%d)"), itr->_key.Str(), itr->_data);

            if (itr->_data == 2)
            {
                HE_LOG_LINE(HE_STR_TEXT("Delete data 2"));
                srcmap.Erase(itr);
            }
        }

        // 正しく削除されているかチェック
        EXPECT_EQ(srcmap.Contains(HE_STR_TEXT("b")), FALSE);
        EXPECT_EQ(srcmap.Size(), 5);
    }
}  // namespace Core
