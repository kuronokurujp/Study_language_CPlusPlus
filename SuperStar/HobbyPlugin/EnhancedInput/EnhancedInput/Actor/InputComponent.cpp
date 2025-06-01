#include "InputComponent.h"

namespace EnhancedInput
{
    InputComponent::InputComponent(Core::Memory::SharedPtr<InputStrategyBase> in_pStrategy)
        : Actor::Component()
    {
        this->_spStrategy = in_pStrategy;
    }

    InputComponent::~InputComponent()
    {
        if (this->_spStrategy) this->_spStrategy = NULL;
    }

    HE::Bool InputComponent::VEnd()
    {
        if (this->_spStrategy)
        {
            this->_spStrategy = NULL;
        }

        return Component::VEnd();
    }

    void InputComponent::ProcessInput(const InputMap& in_mInputMap)
    {
        HE_ASSERT_RETURN(this->_spStrategy);

        this->_spStrategy->VProcessInput(in_mInputMap, this->_pOwner);
    }
    /*
        void InputComponent::SetStrategy(std::shared_ptr<InputStrategyBase> in_pStrategy)
        {
            this->_spStrategy = in_pStrategy;
        }
        */

}  // namespace EnhancedInput
