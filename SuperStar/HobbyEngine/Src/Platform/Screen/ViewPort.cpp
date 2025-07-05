#include "Engine/Platform/Screen/ViewPort.h"

#include "Engine/Platform/Screen/Scene.h"

namespace Platform
{
    ViewPortStrategy::ViewPortStrategy(const ViewPortConfig& in_rConfig)
    {
        this->_config = in_rConfig;
    }

    void ViewPortStrategy::VBegin()
    {
        for (auto it = this->_mRender.Begin(); it != this->_mRender.End(); ++it)
        {
            it->_data->VBegin();
        }

        for (auto it = this->_mSceneSt.Begin(); it != this->_mSceneSt.End(); ++it)
        {
            it->_data->VBegin();
        }
    }

    void ViewPortStrategy::VEnd()
    {
        for (auto it = this->_mSceneSt.Begin(); it != this->_mSceneSt.End(); ++it)
        {
            it->_data->VEnd();
            it->_data.reset();
        }
        this->_mSceneSt.Clear();

        for (auto it = this->_mRender.Begin(); it != this->_mRender.End(); ++it)
        {
            it->_data->VEnd();
            // TODO: レンダリングインターフェイスの解放
            // 以降は使えなくなるので注意
            it->_data.reset();
        }
        this->_mRender.Clear();
    }

    void ViewPortStrategy::Render()
    {
        // レンダリング前
        this->_VBeginRender();

        // TODO: シーンのレンダリング
        for (auto it = this->_mSceneSt.Begin(); it != this->_mSceneSt.End(); ++it)
        {
            it->_data->VRender();
        }

        // レンダリング後
        this->_VEndRender();
    }

    Core::Memory::SharedPtr<Platform::SceneStrategy> ViewPortStrategy::GetScene(const Core::Common::Handle in_handle)
    {
        return this->_mSceneSt.FindKey(in_handle)->_data;
    }
}  // namespace Platform
