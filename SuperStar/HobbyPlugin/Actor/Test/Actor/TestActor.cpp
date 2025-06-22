#include <gtest/gtest.h>

#include "Actor/Actor.h"
#include "Actor/ActorManager.h"
#include "Actor/Component/Component.h"

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

    /// <summary>
    /// 派生チェック用のクラス
    /// </summary>
    class CheckRTTI
    {
        HE_GENERATED_CLASS_BASE_BODY_HEADER(CheckRTTI);

    public:
        CheckRTTI()          = default;
        virtual ~CheckRTTI() = default;
    };

    class SimpleObject : public Object
    {
        // 以下を入れないとObject型で派生した全クラスに対して派生対象とみなされる
        HE_GENERATED_CLASS_BODY_HEADER(SimpleObject, Object);

    public:
        SimpleObject(const HE::Uint32 in_uGroupId)
            : Object(in_uGroupId)
        {
        }
    };

    class SimpleObject2 : public Object
    {
        // 以下を入れないとObject型で派生した全クラスに対して派生対象とみなされる
        HE_GENERATED_CLASS_BODY_HEADER(SimpleObject2, Object);

    public:
        SimpleObject2(const HE::Uint32 in_uGroupId)
            : Object(in_uGroupId)
        {
        }
    };

    class SimpleObject3 : public Object
    {
    public:
        SimpleObject3(const HE::Uint32 in_uGroupId)
            : Object(in_uGroupId)
        {
        }
    };

    // Actorオブジェクトのシンプルオブジェクトでテスト
    TEST(HobbyPlugin_Actor, ActorSimpleObject)
    {
        auto simpleObj =
            SimpleObject(0);
        // SimpleObjectはObjectの派生クラスであることを確認
        EXPECT_EQ(HE_GENERATED_CHECK_RTTI(simpleObj, Object), TRUE);
        // SimpleObjectはObjectの派生クラスであるが, SimpleObject2の派生クラスではないことを確認
        EXPECT_NE(HE_GENERATED_CHECK_RTTI(simpleObj, SimpleObject2), TRUE);

        auto simpleObj2 =
            SimpleObject2(0);
        // SimpleObject2はObjectの派生クラスであるが、SimpleObjectの派生クラスではないことを確認
        EXPECT_NE(HE_GENERATED_CHECK_RTTI(simpleObj2, SimpleObject), TRUE);
        EXPECT_EQ(HE_GENERATED_CHECK_RTTI(simpleObj2, Object), TRUE);

        class SpecialObject : public SimpleObject
        {
            // RTTIを生成するためのマクロを入れなくても良いが
        public:
            SpecialObject(const HE::Uint32 in_uGroupId)
                : SimpleObject(in_uGroupId)
            {
            }
        };

        // SpecialObjectはSimpleObjectの派生クラスであることを確認
        auto specialObj =
            SpecialObject(0);

        // SpecialObjectはObjectの派生クラスであることを確認
        EXPECT_EQ(HE_GENERATED_CHECK_RTTI(specialObj, Object), TRUE);
        // SpecialObjectはSimpleObjectの派生クラスであることを確認
        EXPECT_EQ(HE_GENERATED_CHECK_RTTI(specialObj, SimpleObject), TRUE);
        // SpecialObjectはCheckRTTIの派生クラスではないことを確認
        EXPECT_NE(HE_GENERATED_CHECK_RTTI(specialObj, CheckRTTI), TRUE);
        // SpecialObjectクラスはRTTIを定義するマクロを使用していないためのでコンパイルエラーになる
        //EXPECT_EQ(HE_GENERATED_CHECK_RTTI(specialObj, SpecialObject), TRUE);
        EXPECT_NE(HE_GENERATED_CHECK_RTTI(specialObj, SimpleObject2), TRUE);

        auto simpleObj3 =
            SimpleObject3(0);

        EXPECT_EQ(HE_GENERATED_CHECK_RTTI(simpleObj3, Object), TRUE);
        EXPECT_NE(HE_GENERATED_CHECK_RTTI(simpleObj3, SimpleObject), TRUE);
    }
}  // namespace Actor
