#include <gtest/gtest.h>

#include "Engine/Common/PoolManager.h"

namespace Core
{
    TEST(HobbyEngine_Common, PoolManagerFixPoolManagerBasic)
    {
        Core::Common::FixedPoolManager<HE::Uint32, 32> poolManagerWithUint32;

        // 割り当てして正常かチェック
        auto [handle, pNumber] = poolManagerWithUint32.Alloc();
        EXPECT_NE(pNumber, NULL);
        EXPECT_EQ(handle.Null(), FALSE);
        EXPECT_EQ(handle.Index(), 0);

        (*pNumber) = 32;
        EXPECT_EQ((*pNumber), 32);
        EXPECT_EQ(poolManagerWithUint32.Size(), 1);

        // 割り当てたのを解放して正常チェック
        EXPECT_EQ(poolManagerWithUint32.Free(handle), TRUE);
        EXPECT_EQ(poolManagerWithUint32.Size(), 0);
    }

    TEST(HobbyEngine_Common, PoolManagerFixPoolManagerMaxAlloc)
    {
        Core::Common::FixedPoolManager<HE::Uint32, 3> poolManagerWithUint32;

        // 最大まで割り当てをする
        for (HE::Uint32 i = 0; i < 3; ++i)
        {
            auto [handle, pNumber] = poolManagerWithUint32.Alloc();
            EXPECT_NE(pNumber, NULL);
            EXPECT_EQ(handle.Null(), FALSE);
            handle.Clear();
        }

        // プールした最大まで割り当てをしたので取得失敗
        auto [handle, pNumber] = poolManagerWithUint32.Alloc();
        EXPECT_EQ(pNumber, NULL);
    }

    TEST(HobbyEngine_Common, PoolManagerFixPoolManagerHandleCheckIndex)
    {
        Core::Common::FixedPoolManager<HE::Uint32, 3> poolManagerWithUint32;

        // 最大まで割り当てをする
        {
            auto [handle, pNumber] = poolManagerWithUint32.Alloc();
            EXPECT_NE(pNumber, NULL);
            EXPECT_EQ(handle.Null(), FALSE);
            EXPECT_EQ(handle.Index(), 0);
        }
        {
            auto [handle, pNumber] = poolManagerWithUint32.Alloc();
            EXPECT_NE(pNumber, NULL);
            EXPECT_EQ(handle.Null(), FALSE);
            EXPECT_EQ(handle.Index(), 1);

            EXPECT_EQ(poolManagerWithUint32.Free(handle), TRUE);
        }

        {
            auto [handle, pNumber] = poolManagerWithUint32.Alloc();
            EXPECT_NE(pNumber, NULL);
            EXPECT_EQ(handle.Null(), FALSE);
            EXPECT_EQ(handle.Index(), 1);
        }

        {
            auto [handle, pNumber] = poolManagerWithUint32.Alloc();
            EXPECT_NE(pNumber, NULL);
            EXPECT_EQ(handle.Null(), FALSE);
            EXPECT_EQ(handle.Index(), 2);
        }
    }

    // 全削除可能かチェック
    TEST(HobbyEngine_Common, PoolManagerFixPoolManagerAllDelete)
    {
        Core::Common::FixedPoolManager<HE::Uint32, 3> poolManagerWithUint32;

        // 最大まで割り当てをする
        {
            auto [handle, pNumber] = poolManagerWithUint32.Alloc();
            EXPECT_NE(pNumber, NULL);
            EXPECT_EQ(handle.Null(), FALSE);
            EXPECT_EQ(handle.Index(), 0);
        }
        {
            auto [handle, pNumber] = poolManagerWithUint32.Alloc();
            EXPECT_NE(pNumber, NULL);
            EXPECT_EQ(handle.Null(), FALSE);
            EXPECT_EQ(handle.Index(), 1);

            EXPECT_EQ(poolManagerWithUint32.Free(handle), TRUE);
        }

        {
            auto [handle, pNumber] = poolManagerWithUint32.Alloc();
            EXPECT_NE(pNumber, NULL);
            EXPECT_EQ(handle.Null(), FALSE);
            EXPECT_EQ(handle.Index(), 1);
        }

        {
            auto [handle, pNumber] = poolManagerWithUint32.Alloc();
            EXPECT_NE(pNumber, NULL);
            EXPECT_EQ(handle.Null(), FALSE);
            EXPECT_EQ(handle.Index(), 2);
        }

        auto m = poolManagerWithUint32.GetUseDataMap();
        for (auto itr = m.Begin(); itr != m.End(); ++itr)
        {
            EXPECT_EQ(poolManagerWithUint32.Free(itr->_key), TRUE);
        }
        EXPECT_EQ(poolManagerWithUint32.Empty(), TRUE);
    }
}  // namespace Core
