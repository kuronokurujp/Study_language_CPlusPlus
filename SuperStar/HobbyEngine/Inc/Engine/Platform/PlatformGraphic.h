#pragma once

#include <functional>

#include "Engine/Common/CustomMap.h"
#include "Engine/Common/Handle.h"
#include "Engine/Math/Math.h"
#include "Engine/MiniEngine.h"

// グラフィック関連
#include "Engine/Platform/Graphic/Drawable.h"
#include "Engine/Platform/Graphic/FrameBase.h"

// プラットフォームのグラフィック機能
namespace Platform
{
    /// <summary>
    /// グラフィックのインターフェイス
    /// </summary>
    class GraphicInterface
    {
    public:
    public:
        virtual ~GraphicInterface() = default;
        virtual void VRelease()     = 0;

        /// <summary>
        /// 更新
        /// </summary>
        virtual void VUpdate(const HE::Float32) = 0;

        /// <summary>
        /// 描画
        /// </summary>
        virtual void VDraw() = 0;

        /// <summary>
        /// フレームを作成
        /// </summary>
        virtual Core::Common::Handle VCreateFrame(const FrameConfig&,
                                                  Core::Memory::UniquePtr<EventFrameInterface>) = 0;
        virtual FrameBase* VGetFrame(const Core::Common::Handle)                                = 0;

        /// <summary>
        /// 描画可能なオブジェクトを追加
        /// 継承したクラスで追加する
        /// 同じオブジェクトを何度も追加することはできない
        /// </summary>
        virtual void VAddDrawable(Core::Memory::UniquePtr<Drawable2DInterface>) = 0;
    };
}  // namespace Platform
