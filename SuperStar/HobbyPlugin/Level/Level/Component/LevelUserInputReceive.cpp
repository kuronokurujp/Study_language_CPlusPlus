#include "LevelUserInputReceive.h"

namespace Level
{
    void LevelUserInputReceiveComponent::VSetup(const HE::Bool in_bAutoDelete)
    {
        LevelBaseComponent::VSetup(in_bAutoDelete);
    }

    HE::Bool LevelUserInputReceiveComponent::VEnd()
    {
        HE_SAFE_DELETE_UNIQUE_PTR(this->_spEventReceiver);
        return LevelBaseComponent::VEnd();
    }

    void LevelUserInputReceiveComponent::Message(const HE::Char* in_szMsg)
    {
        this->_spEventReceiver->Message(in_szMsg);
    }

}  // namespace Level
