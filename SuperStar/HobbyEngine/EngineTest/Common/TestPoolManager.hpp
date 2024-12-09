#pragma once

#include "Engine/Common/PoolManager.h"

TEST_CASE("PoolManager FixPoolManager Basic")
{
    Core::Common::FixedPoolManager<HE::Uint32, 32> poolManagerWithUint32;

    // 割り当てして正常かチェック
    auto [handle, pNumber] = poolManagerWithUint32.Alloc();
    CHECK(pNumber != NULL);
    CHECK(handle.Null() == FALSE);
    CHECK(handle.Index() == 0);

    (*pNumber) = 32;
    CHECK((*pNumber) == 32);
    CHECK(poolManagerWithUint32.Size() == 1);

    // 割り当てたのを解放して正常チェック
    CHECK(poolManagerWithUint32.Free(handle));
    CHECK(poolManagerWithUint32.Size() == 0);
}

TEST_CASE("PoolManager FixPoolManager MaxAlloc")
{
    Core::Common::FixedPoolManager<HE::Uint32, 3> poolManagerWithUint32;

    // 最大まで割り当てをする
    for (HE::Uint32 i = 0; i < 3; ++i)
    {
        auto [handle, pNumber] = poolManagerWithUint32.Alloc();
        CHECK(pNumber != NULL);
        CHECK(handle.Null() == FALSE);
        handle.Clear();
    }

    // プールした最大まで割り当てをしたので取得失敗
    auto [handle, pNumber] = poolManagerWithUint32.Alloc();
    CHECK(pNumber == NULL);
}

TEST_CASE("PoolManager FixPoolManager Handle Check Index")
{
    Core::Common::FixedPoolManager<HE::Uint32, 3> poolManagerWithUint32;

    // 最大まで割り当てをする
    {
        auto [handle, pNumber] = poolManagerWithUint32.Alloc();
        CHECK(pNumber != NULL);
        CHECK(handle.Null() == FALSE);
        CHECK(handle.Index() == 0);
    }
    {
        auto [handle, pNumber] = poolManagerWithUint32.Alloc();
        CHECK(pNumber != NULL);
        CHECK(handle.Null() == FALSE);
        CHECK(handle.Index() == 1);

        CHECK(poolManagerWithUint32.Free(handle));
    }

    {
        auto [handle, pNumber] = poolManagerWithUint32.Alloc();
        CHECK(pNumber != NULL);
        CHECK(handle.Null() == FALSE);
        CHECK(handle.Index() == 1);
    }

    {
        auto [handle, pNumber] = poolManagerWithUint32.Alloc();
        CHECK(pNumber != NULL);
        CHECK(handle.Null() == FALSE);
        CHECK(handle.Index() == 2);
    }
}

// 全削除可能かチェック
TEST_CASE("PoolManager FixPoolManager AllDelete")
{
    Core::Common::FixedPoolManager<HE::Uint32, 3> poolManagerWithUint32;

    // 最大まで割り当てをする
    {
        auto [handle, pNumber] = poolManagerWithUint32.Alloc();
        CHECK(pNumber != NULL);
        CHECK(handle.Null() == FALSE);
        CHECK(handle.Index() == 0);
    }
    {
        auto [handle, pNumber] = poolManagerWithUint32.Alloc();
        CHECK(pNumber != NULL);
        CHECK(handle.Null() == FALSE);
        CHECK(handle.Index() == 1);

        CHECK(poolManagerWithUint32.Free(handle));
    }

    {
        auto [handle, pNumber] = poolManagerWithUint32.Alloc();
        CHECK(pNumber != NULL);
        CHECK(handle.Null() == FALSE);
        CHECK(handle.Index() == 1);
    }

    {
        auto [handle, pNumber] = poolManagerWithUint32.Alloc();
        CHECK(pNumber != NULL);
        CHECK(handle.Null() == FALSE);
        CHECK(handle.Index() == 2);
    }

    auto m = poolManagerWithUint32.GetUseDataMap();
    for (auto itr = m.Begin(); itr != m.End(); ++itr)
    {
        CHECK(poolManagerWithUint32.Free(itr->key));
    }
    CHECK(poolManagerWithUint32.Empty());
}
