#include "Scene.h"

#include "Engine/Platform/PlatformModule.h"

namespace Render
{
    HE::Bool SceneViewBase::_Begin(
        Core::Memory::UniquePtr<Platform::SceneStrategyInterface> in_upSt)
    {
        this->_upSt = std::move(in_upSt);
        return TRUE;
    }

    void SceneViewBase::Release()
    {
        HE_SAFE_DELETE_UNIQUE_PTR(this->_upSt);
    }

    void SceneViewBase::_End()
    {
    }

    void SceneViewBase::_Update(const HE::Float32)
    {
    }

    void SceneViewBase::_BeginRender()
    {
        this->_upSt->VBeginRender();
    }

    void SceneViewBase::_EndRender()
    {
        this->_upSt->VEndRender();
    }

    HE::Bool SceneViewBase::_PushCommand(Command&& in_rrCmd)
    {
        // コマンドをコピー
        // 要素を先頭に追加
        // 最後に追加したコマンドは配列先頭に
        // 最初に追加したコマンドは配列末尾に
        this->_commandBuff.PushFront(in_rrCmd);

        return TRUE;
    }
}  // namespace Render
