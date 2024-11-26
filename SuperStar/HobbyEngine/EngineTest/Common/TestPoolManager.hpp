#pragma once

#include "Engine/Common/PoolManager.h"

TEST_CASE("PoolManager FixPoolManager Basic")
{
    Core::Common::FixedPoolManager<Uint32, 32> poolManagerWithUint32;

    // 割り当てして正常かチェック
    Core::Common::Handle handle;
    auto* pNumber = poolManagerWithUint32.Alloc(&handle);
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
    Core::Common::FixedPoolManager<Uint32, 3> poolManagerWithUint32;

    // 最大まで割り当てをする
    Core::Common::Handle handle;
    for (Uint32 i = 0; i < 3; ++i)
    {
        auto* pNumber = poolManagerWithUint32.Alloc(&handle);
        CHECK(pNumber != NULL);
        CHECK(handle.Null() == FALSE);
        handle.Clear();
    }

    // プールした最大まで割り当てをしたので取得失敗
    auto* pNumber = poolManagerWithUint32.Alloc(&handle);
    CHECK(pNumber == NULL);
}

TEST_CASE("PoolManager FixPoolManager Handle Check Index")
{
    Core::Common::FixedPoolManager<Uint32, 3> poolManagerWithUint32;

    // 最大まで割り当てをする
    Core::Common::Handle handle;
    auto* pNumber = poolManagerWithUint32.Alloc(&handle);
    CHECK(pNumber != NULL);
    CHECK(handle.Null() == FALSE);
    CHECK(handle.Index() == 0);

    handle.Clear();
    pNumber = poolManagerWithUint32.Alloc(&handle);
    CHECK(pNumber != NULL);
    CHECK(handle.Null() == FALSE);
    CHECK(handle.Index() == 1);

    CHECK(poolManagerWithUint32.Free(handle));

    handle.Clear();
    pNumber = poolManagerWithUint32.Alloc(&handle);
    CHECK(pNumber != NULL);
    CHECK(handle.Null() == FALSE);
    CHECK(handle.Index() == 1);

    handle.Clear();
    pNumber = poolManagerWithUint32.Alloc(&handle);
    CHECK(pNumber != NULL);
    CHECK(handle.Null() == FALSE);
    CHECK(handle.Index() == 2);
}
