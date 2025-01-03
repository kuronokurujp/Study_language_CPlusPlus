#pragma once

#include "Engine/Core.h"
#include "Engine/Memory/Memory.h"

// 前方宣言
namespace Platform
{
    class TimeInterface;
}

/// <summary>
/// ゲーム中のFPS制御
/// </summary>
namespace Core::Time
{
    class FPS
    {
        HE_CLASS_DEFAULT_CONSTRUCT_NG(FPS);

    public:
        FPS(Core::Memory::WeakPtr<Platform::TimeInterface>);

        /// <summary>
        /// 固定フレームモード有効化
        /// </summary>
        void EnableFixedMode(const HE::Uint32 in_uFPS)
        {
            this->_uFixedFPS       = in_uFPS;
            this->_uFixedFrameMSec = static_cast<HE::Uint32>(
                (1.0f / static_cast<HE::Float32>(this->_uFixedFPS)) * 1000.0f);
        }

        /// <summary>
        /// 固定フレームモードを無効化
        /// </summary>
        void DisableFixedMode()
        {
            this->_uFixedFPS       = 0;
            this->_uFixedFrameMSec = 0;
        }

        /// <summary>
        /// 固定フレームモードかどうか
        /// </summary>
        inline HE::Bool IsFixedMode() const { return (this->_uFixedFPS != 0); }

        HE::Bool IsWaitFrameFixedMode(Core::Memory::WeakPtr<Platform::TimeInterface>);

        /// <summary>
        /// 固定フレームモードでの1フレームのMSec時間
        /// </summary>
        inline HE::Uint32 GetFrameMSecByFixedMode() const { return this->_uFixedFrameMSec; }

        /// <summary>
        /// 時間更新
        /// </summary>
        HE::Bool UpdateTime(Core::Memory::WeakPtr<Platform::TimeInterface> in_wpTimeInterface);

        /// <summary>
        /// 更新時間取得(秒)
        /// </summary>
        HE::Float32 GetDeltaTimeSec() const;
        HE::Float32 GetDeltaTimeMSec() const;

        inline HE::Uint32 GetFrameRate() const { return this->_uFrameRate; }
        inline HE::Uint32 GetLastTimeMSec() const
        {
            return this->_uaPreviousTimeMSec[FPS::_uTimeAvgCount - 1];
        }

    private:
        static constexpr HE::Uint32 _uTimeAvgCount       = 10;
        static constexpr HE::Float64 _ffFrameAvgTimeMSec = 1000.0f * _uTimeAvgCount;

        HE::Uint32 _uaPreviousTimeMSec[_uTimeAvgCount] = {0};
        HE::Float32 _uFrameRate                        = 0;
        HE::Uint32 _uCurrentTime                       = 0;
        HE::Uint32 _uFixedFPS                          = 0;
        HE::Uint32 _uFixedFrameMSec                    = 0;
    };
}  // namespace Core::Time
