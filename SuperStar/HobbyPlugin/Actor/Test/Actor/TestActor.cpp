#include <gtest/gtest.h>

#include "Actor/Actor.h"
#include "Actor/ActorManager.h"

namespace Actor
{
    TEST(HobbyPlugin_Actor, ActorManagerSimple)
    {
        auto actorManger = ActorManager();
        auto bRet        = ACTOR_MANAGER_START(actorManger, 32, 2);
        EXPECT_EQ(bRet, TRUE);

        actorManger.Release();
    }

    TEST(HobbyPlugin_Actor, ActorManagerAddActor)
    {
        auto actorManger = ActorManager();
        auto bRet        = ACTOR_MANAGER_START(actorManger, 32, 1);
        EXPECT_EQ(bRet, TRUE);

        auto handle = actorManger.Add<Object>(0);
        EXPECT_EQ(handle.Null(), FALSE);

        actorManger.Release();
    }
}  // namespace Actor
