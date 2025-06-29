#pragma once

#include "Engine/Common/CustomMap.h"
#include "Engine/Memory/Memory.h"
#include "Engine/MiniEngine.h"
#include "Engine/Platform/Screen/Render.h"

namespace Platform
{
    // 前方宣言
    class SceneStrategy;
    struct SceneConfig;

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
                                                  Core::Memory::UniquePtr<SceneStrategy>, 32>;

        using MapRender = Core::Common::FixedMap<const Core::Common::RTTI*,
                                                 Core::Memory::UniquePtr<RenderInterface>, 32>;

        using EventRender = std::function<void(RenderInterface*, const ViewPortConfig&)>;

    public:
        ViewPortStrategy(const ViewPortConfig&);

        virtual void VBegin();
        virtual void VEnd();

        virtual void VBeginRender() = 0;
        virtual void VEndRender()   = 0;

        // TODO: レンダリングインターフェイスを継承しているかチェックを入れる
        template <class T>
        Core::Common::Handle CreateScene(const SceneConfig& in_rConfig, EventRender in_eventRender)
        {
            HE_STATIC_ASSERT(std::is_base_of<RenderInterface, T>::value,
                             "TクラスはRenderInterfaceクラスを継承していない");

            // TODO:レンダリングがすでに存在しているかチェックをしてあれば再利用なければ新規生成
            {
                auto pAddr = T::StaticRTTI();
                if (this->_mRender.Contains(pAddr))
                {
                }
                else
                {
                    this->_mRender.Add(pAddr, std::move(HE_MAKE_CUSTOM_UNIQUE_PTR((T))));
                }
            }

            // TODO: シーンにレンダリングを渡す

            auto upScene =
                this->_VCreateScene(in_rConfig);
                /*
                                    [this, std::move(in_eventRender)]()
                                    {
                                        auto pRender =
                                            this->_mRender.FindKey(T::StaticRTTI()).data->get();
                                        in_eventRender(pRender, this->_config);
                                    });
                                    */

            Core::Common::Handle handle;
            handle.SetIndex(++this->_uScenetStCount, 0);
            this->_mSceneSt.Add(handle, std::move(upScene));
            return handle;
        }

        inline const ViewPortConfig& GetConfig() const { return this->_config; }

    protected:
        /// <summary>
        /// TODO: Sceneの生成は継承先へ
        /// </summary>
        virtual Core::Memory::UniquePtr<Platform::SceneStrategy> _VCreateScene(
            const SceneConfig&) = 0;

    protected:
        ViewPortConfig _config;

        MapSceneSt _mSceneSt;
        MapRender _mRender;
        HE::Uint32 _uScenetStCount = 0;
    };
}  // namespace Platform
