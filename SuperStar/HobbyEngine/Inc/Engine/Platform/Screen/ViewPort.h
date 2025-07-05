#pragma once

#include "Engine/Common/CustomMap.h"
#include "Engine/Memory/Memory.h"
#include "Engine/MiniEngine.h"
#include "Engine/Platform/Screen/Render.h"
#include "Engine/Platform/Screen/Scene.h"

namespace Platform
{
    struct ViewPortConfig
    {
        HE::Uint32 _uWidth  = 0;
        HE::Uint32 _uHeight = 0;
    };

    /// <summary>
    /// ビューポートのロジック
    /// </summary>
    class ViewPortStrategy
    {
    private:
        using MapSceneSt = Core::Common::FixedMap<Core::Common::Handle,
                                                  Core::Memory::SharedPtr<SceneStrategy>, 32>;

        using MapRender = Core::Common::FixedMap<const Core::Common::RTTI*,
                                                 Core::Memory::SharedPtr<RenderInterface>, 32>;

    public:
        ViewPortStrategy(const ViewPortConfig&);

        virtual void VBegin();
        virtual void VEnd();

        void Render();

        // TODO: レンダリングインターフェイスを継承しているかチェックを入れる
        // TODO: シーン個別で設定するのが今はない
        template <class T>
        Core::Common::Handle CreateScene(SceneStrategy::EventRender in_eventRender)
        {
            HE_STATIC_ASSERT(std::is_base_of<RenderInterface, T>::value,
                             "TクラスはRenderInterfaceクラスを継承していない");

            // TODO:レンダリングがすでに存在しているかチェックをしてあれば再利用なければ新規生成
            Core::Memory::SharedPtr<RenderInterface> spNewRender = NULL;
            {
                auto pAddr = T::StaticRTTI();
                if (this->_mRender.Contains(pAddr) == FALSE)
                {
                    spNewRender = HE_MAKE_CUSTOM_SHARED_PTR((T));
                    this->_mRender.Add(pAddr, spNewRender);
                }
                else
                {
                    spNewRender = this->_mRender.FindKey(pAddr)->_data;
                }
            }

            // TODO: シーンにレンダリングを渡す
            SceneConfig config = {this->_config._uWidth, this->_config._uHeight};
            auto upScene = this->_VCreateScene(config, spNewRender, std::move(in_eventRender));

            Core::Common::Handle handle;
            handle.SetIndex(++this->_uScenetStCount, 0);
            this->_mSceneSt.Add(handle, upScene);
            return handle;
        }

        inline const ViewPortConfig& GetConfig() const { return this->_config; }

        Core::Memory::SharedPtr<Platform::SceneStrategy> GetScene(const Core::Common::Handle);

    protected:
        virtual void _VBeginRender() = 0;
        virtual void _VEndRender()   = 0;

        /// <summary>
        /// TODO: Sceneの生成は継承先へ
        /// </summary>
        virtual Core::Memory::SharedPtr<Platform::SceneStrategy> _VCreateScene(
            const SceneConfig&, Core::Memory::SharedPtr<RenderInterface>,
            SceneStrategy::EventRender) = 0;

    protected:
        ViewPortConfig _config;

    private:
        MapSceneSt _mSceneSt;
        MapRender _mRender;
        HE::Uint32 _uScenetStCount = 0;
    };
}  // namespace Platform
