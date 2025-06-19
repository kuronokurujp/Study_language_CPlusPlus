#include <gtest/gtest.h>

#include "Engine/Common/CustomArray.h"
#include "Engine/Memory/Memory.h"

namespace Core
{
    /// <summary>
    /// 初期化と終了が成功するか
    /// </summary>
    TEST(HobbyEngine_Memory, MemoryInittoEnd)
    {
        Core::Memory::Manager memoryManager;

        EXPECT_EQ(memoryManager.Start(0x1000000), TRUE);
        EXPECT_EQ(memoryManager.VRelease(), TRUE);

        memoryManager.Reset();
    }

    /// <summary>
    /// メモリ設定が成功するか
    /// </summary>
    TEST(HobbyEngine_Memory, MemorySetupMemory)
    {
        Core::Memory::Manager memoryManager;
        EXPECT_EQ(memoryManager.Start(0x1000000), TRUE);

        // ページ確保テスト
        {
            // メモリサイズのイニシャライズ
            Core::Memory::Manager::PageSetupInfo memoryPageSetupInfoArray[] = {
                // 複数ページのサイズ
                {0, 3 * 1024 * 1024}, {1, 4 * 1024 * 1024}, {2, 2 * 1024 * 1024},
                {3, 2 * 1024 * 1024}, {4, 2 * 1024 * 1024}, {5, 3 * 1024 * 1024},
            };

            EXPECT_EQ(memoryManager.SetupMemoryPage(memoryPageSetupInfoArray,
                                                    HE_ARRAY_NUM(memoryPageSetupInfoArray)),
                      TRUE);
            EXPECT_EQ(memoryManager.CheckAllMemoryBlock(), TRUE);
        }

        EXPECT_EQ(memoryManager.VRelease(), TRUE);
        memoryManager.Reset();
    }

    /// <summary>
    /// メモリ確保とメモリ解放を繰り返してもメモリ状態が正しくなるか
    /// </summary>
    TEST(HobbyEngine_Memory, MemoryAllocateAndFree)
    {
        Core::Memory::Manager memoryManager;
        EXPECT_EQ(memoryManager.VRelease(), TRUE);
        EXPECT_EQ(memoryManager.Start(0x1000000), TRUE);

        // ページ確保テスト
        HE::Uint8 pageMax = 0;
        {
            // メモリサイズのイニシャライズ
            Core::Memory::Manager::PageSetupInfo memoryPageSetupInfoArray[] = {
                // 複数ページのサイズ
                {0, 3 * 1024 * 1024}, {1, 4 * 1024 * 1024}, {2, 2 * 1024 * 1024},
                {3, 2 * 1024 * 1024}, {4, 2 * 1024 * 1024}, {5, 3 * 1024 * 1024},
            };

            pageMax = HE_ARRAY_NUM(memoryPageSetupInfoArray);
            EXPECT_EQ(memoryManager.SetupMemoryPage(memoryPageSetupInfoArray, pageMax), TRUE);
            EXPECT_EQ(memoryManager.CheckAllMemoryBlock(), TRUE);
        }

        // 確保と解放を繰り返して確保したのが残らないかテスト
        memoryManager.PrintAllMemoryInfo();
        for (HE::Uint32 i = 0; i < 100; ++i)
        {
            HE::Uint8 page = i % pageMax;

            // メモリの前確保がうまくいっているか
            HE::Uint8* pPtr = static_cast<HE::Uint8*>(HE_ALLOC_MEM(0x10000, page));
            EXPECT_NE(pPtr, NULL);
            {
                *pPtr = static_cast<HE::Uint8>(i);
                HE_LOG_LINE(HE_STR_TEXT("メモリページ(%d)の前確保したメモリに設定した値(%d)"), page,
                            *pPtr);
            }

            // メモリの後確保がうまくいっているか
            HE::Uint32* pPtr2 = static_cast<HE::Uint32*>(HE_ALLOC_MEM_LAST(1000, 1));
            EXPECT_NE(pPtr2, NULL);
            {
                *pPtr2 = i + 1234;
                HE_LOG_LINE(HE_STR_TEXT("メモリページ(%d)の後確保したメモリに設定した値(%d)"), page,
                            *pPtr2);
            }

            HE_SAFE_DELETE_MEM(pPtr);
            HE_SAFE_DELETE_MEM(pPtr2);
        }
        memoryManager.PrintAllMemoryInfo();

        EXPECT_EQ(memoryManager.VRelease(), TRUE);
        memoryManager.Reset();
    }

    TEST(HobbyEngine_Memory, MemoryCustomShaderPtr)
    {
        Core::Memory::Manager memoryManager;
        EXPECT_EQ(memoryManager.Start(0x1000000), TRUE);

        // ページ確保テスト
        {
            // メモリサイズのイニシャライズ
            Core::Memory::Manager::PageSetupInfo memoryPageSetupInfoArray[] = {
                // 複数ページのサイズ
                {0, 3 * 1024 * 1024}, {1, 4 * 1024 * 1024}, {2, 2 * 1024 * 1024},
                {3, 2 * 1024 * 1024}, {4, 2 * 1024 * 1024}, {5, 3 * 1024 * 1024},
            };

            EXPECT_EQ(memoryManager.SetupMemoryPage(memoryPageSetupInfoArray,
                                                    HE_ARRAY_NUM(memoryPageSetupInfoArray)),
                      TRUE);
            EXPECT_EQ(memoryManager.CheckAllMemoryBlock(), TRUE);
        }

        // 共有ポインターのメモリ確報がうまくいっているかチェック
        struct Data
        {
            Data()
            {
                HE_LOG_LINE(
                    HE_STR_TEXT("HE_MEMのメモリ確保でData型のコンストラクターが呼ばれている"));
                this->i = 0;
            };
            Data(HE::Uint32 a)
            {
                HE_LOG_LINE(
                    HE_STR_TEXT("HE_MEMのメモリ確保でData型のコンストラクターが呼ばれている"));
                this->i = a;
            }
            ~Data()
            {
                HE_LOG_LINE(
                    HE_STR_TEXT("HE_MEMのメモリ確保でData型のデストラクターが呼ばれている"));
            }

            HE::Sint8 i  = 0;
            HE::Sint8 t  = 0;
            HE::Sint8 t2 = 0;
            HE::Sint8 a[12];
        };

        Core::Common::FixedArray<std::shared_ptr<Data>, 10> memArray;
        for (HE::Uint32 i = 0; i < 10; ++i)
        {
            auto p = HE_MAKE_CUSTOM_SHARED_PTR((Data), i);
            memArray.Set(i, p);
        }

        for (HE::Uint32 i = 0; i < 10; ++i)
        {
            HE_LOG_LINE(HE_STR_TEXT("メモリページ(%d)の解放"), memArray[i]->i);
            EXPECT_EQ(i, memArray[i]->i);
            memArray[i].reset();
        }

        auto pData = HE_NEW_MEM(Data, 0);
        HE_SAFE_DELETE_MEM(pData);

        // class DataClass
        struct DataClass
        {
        public:
            DataClass() { HE_LOG_LINE(HE_STR_TEXT("DataClass型のコンストラクターが呼ばれている")); }
            // TODO: デストラクターがあるとメモリ配列確保を使うとメモリアドレスが変わる
            ~DataClass() { HE_LOG_LINE(HE_STR_TEXT("DataClass型のデストラクタが呼ばれている")); }

        private:
            HE::Uint8 a[3];
        };
        auto pDataClass = HE_NEW_MEM(DataClass, 0);
        HE_SAFE_DELETE_MEM(pDataClass);

        // new と deleteがうまくいっているかチェック
        EXPECT_EQ(memoryManager.UsedAllBlock(), FALSE);

        EXPECT_EQ(memoryManager.VRelease(), TRUE);
        memoryManager.Reset();
    }

    TEST(HobbyEngine_Memory, MemoryCustomUniqePtr)
    {
        Core::Memory::Manager memoryManager;
        EXPECT_EQ(memoryManager.Start(0x1000000), TRUE);

        // ページ確保テスト
        {
            // メモリサイズのイニシャライズ
            Core::Memory::Manager::PageSetupInfo memoryPageSetupInfoArray[] = {
                // 複数ページのサイズ
                {0, 3 * 1024 * 1024}, {1, 4 * 1024 * 1024}, {2, 2 * 1024 * 1024},
                {3, 2 * 1024 * 1024}, {4, 2 * 1024 * 1024}, {5, 3 * 1024 * 1024},
            };

            EXPECT_EQ(memoryManager.SetupMemoryPage(memoryPageSetupInfoArray,
                                                    HE_ARRAY_NUM(memoryPageSetupInfoArray)),
                      TRUE);
            EXPECT_EQ(memoryManager.CheckAllMemoryBlock(), TRUE);
        }

        // ユニークポインターのメモリ確保と解放がうまくいっているかチェック
        struct Data
        {
            Data() { this->i = 0; };
            Data(HE::Uint32 a) { this->i = a; }
            HE::Sint8 i = 0;
        };

        Core::Common::FixedArray<Core::Memory::UniquePtr<Data>, 10> memArray;
        for (HE::Uint32 i = 0; i < 10; ++i)
        {
            auto p = HE_MAKE_CUSTOM_UNIQUE_PTR((Data), i);
            memArray.Set(i, std::move(p));
        }

        for (HE::Uint32 i = 0; i < 10; ++i)
        {
            HE_LOG_LINE(HE_STR_TEXT("メモリページ(%d)の解放"), memArray[i]->i);
            EXPECT_EQ(i, memArray[i]->i);
            memArray[i].reset();
        }

        // new と deleteがうまくいっているかチェック
        EXPECT_EQ(memoryManager.UsedAllBlock(), FALSE);

        EXPECT_EQ(memoryManager.VRelease(), TRUE);
        memoryManager.Reset();
    }

    TEST(HobbyEngine_Memory, MemoryDefault)
    {
        auto p = HE_ALLOC_MEM(sizeof(HE::Uint32), 0);
        HE_SAFE_DELETE_MEM(p);

        auto p1 = HE_NEW_MEM(HE::Uint64, 0);
        HE_SAFE_DELETE_MEM(p1);
    }
}  // namespace Core
