#include "EnhancedInput/Actor/ActorManagerDecorater.h"

namespace EnhancedInput
{
    HE::Bool ActorManagerDecoraterWithInputSystem::VStart(Actor::ActorManager*)
    {
        this->_lstInputComponent.Clear();

        return TRUE;
    }

    void ActorManagerDecoraterWithInputSystem::ProcessInput(const InputMap& in_mInputMap)
    {
        for (auto it = this->_lstInputComponent.BeginItr(); it != this->_lstInputComponent.EndItr();
             ++it)
        {
            it->ProcessInput(in_mInputMap);
        }
    }

    void ActorManagerDecoraterWithInputSystem::VOnActorRegistComponent(
        Actor::Component* in_pComponent)
    {
        HE_ASSERT(in_pComponent);

        if (in_pComponent->VGetRTTI().DerivesFrom(InputComponent::StaticRTTI()) == FALSE) return;

        auto pInputComponent = reinterpret_cast<InputComponent*>(in_pComponent);
        this->_lstInputComponent.PushBack(*pInputComponent);
    }

    void ActorManagerDecoraterWithInputSystem::VOnActorUnRegistComponent(
        Actor::Component* in_pComponent)
    {
        HE_ASSERT(in_pComponent);
        if (in_pComponent->VGetRTTI().DerivesFrom(InputComponent::StaticRTTI()) == FALSE) return;

        auto pInputComponent = reinterpret_cast<InputComponent*>(in_pComponent);
        this->_lstInputComponent.Erase(pInputComponent);
    }
}  // namespace EnhancedInput
