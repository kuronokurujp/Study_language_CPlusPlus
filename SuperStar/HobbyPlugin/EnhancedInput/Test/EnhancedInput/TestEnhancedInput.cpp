#include <gtest/gtest.h>

#include "Engine//Memory/Memory.h"
#include "EnhancedInput/Actor/ActorManagerDecorater.h"
#include "EnhancedInput/Actor/InputComponent.h"
#include "EnhancedInput/Common.h"
#include "EnhancedInput/Event.h"

namespace EnhancedInput
{
    TEST(HobbyPlugin_EnhancedInput, EnhancedInputActorManager)
    {
        // EnhancedInputのアクター管理デコレーターを使ったアクター管理のテスト

        // 入力システムに対応したアクター管理デコレーターを作成
        auto upDecorater =
            HE_MAKE_CUSTOM_UNIQUE_PTR((EnhancedInput::ActorManagerDecoraterWithInputSystem));

        auto actorManger = Actor::ActorManager(std::move(upDecorater));
        auto bRet        = ACTOR_MANAGER_START(actorManger, 32, 1);
        EXPECT_EQ(bRet, TRUE);

        class TestObject : public Actor::Object
        {
        public:
            TestObject(const HE::Uint32 in_uGroupId) : Actor::Object(in_uGroupId) {}
        };

        // アクターを追加
        auto handle = actorManger.Add<TestObject>(0);
        auto pActor = actorManger.Get(handle);

        class InputStrategy final : public EnhancedInput::InputStrategyBase
        {
        public:
            HE::Bool VBegin() override { return TRUE; }
            HE::Bool VEnd() override { return TRUE; }

            void VProcessInput(const InputMap&, Actor::Object*) override {}
        };

        {
            auto spInputStrategy = HE_MAKE_CUSTOM_SHARED_PTR((InputStrategy));
            auto [handle, pComp] =
                pActor->AddComponentByHandleAndComp<EnhancedInput::InputComponent>(
                    0, Actor::Component::EPriorty_Main, spInputStrategy);
            EXPECT_EQ(handle.Null(), FALSE);
        }

        actorManger.BeginUpdate(0.0f);
        actorManger.Update(0.0f);
        actorManger.LateUpdate(0.0f);

        actorManger.Release();
    }
}  // namespace EnhancedInput
