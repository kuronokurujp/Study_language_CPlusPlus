#include "InGameShotComponent.h"

#include "Actor/Actor.h"

namespace InGame
{
    HE::Bool InGameShotComponent::VEnd()
    {
        this->_spStrategy = NULL;

        return Component::VEnd();
    }

    void InGameShotComponent::VUpdate(const HE::Float32 in_fDt)
    {
        Actor::Component::VUpdate(in_fDt);

        // 弾を打つ
        while (0 < this->_sCatchShot.Size())
        {
            this->_spStrategy->VShot(this->_sCatchShot.PopBack());
        }

        this->_sCatchShot.Clear();
    }

    void InGameShotComponent::Shot(const Core::Math::Vector2& in_rPos,
                                   const Core::Math::Vector2& in_rDir,
                                   const HE::Uint32 in_uCollisionHashCode)
    {
        HE_ASSERT(this->_spStrategy);

        // 利用するデータを確保
        auto pConfig = this->_sCatchShot.PushBack();

        // 確保したデータ設定
        pConfig->pos                = in_rPos;
        pConfig->dir                = in_rDir;
        pConfig->uCollisionHashCoee = in_uCollisionHashCode;
        this->_spStrategy->VConfigure(pConfig);
    }

}  // namespace InGame
