#include "Engine/Time/FPS.h"

#include "Engine/Platform/PlatformModule.h"

namespace Core::Time
{
    FPS::FPS(Core::Memory::WeakPtr<Platform::TimeInterface> in_wpTimeInterface)
    {
        auto pTime = in_wpTimeInterface.lock();
        HE_ASSERT(pTime);

        // 帰ってくる時間の単位はmsec
        auto uCurrentTime = pTime->VNowMSec();
        for (HE::Uint32 i = 0; i < FPS::_uTimeAvgCount; ++i)
            this->_uaPreviousTimeMSec[i] = uCurrentTime;
    }

    HE::Bool FPS::IsWaitFrameFixedMode(
        Core::Memory::WeakPtr<Platform::TimeInterface> in_wpTimeInterface)
    {
        auto pTime = in_wpTimeInterface.lock();
        if (pTime == NULL) return FALSE;

        const HE::Uint64 uNowMSec = pTime->VNowMSec();
        // Waitタイムより早い場合は待機フラグを返す
        HE::Bool bWait = (uNowMSec - this->_uaPreviousTimeMSec[FPS::_uTimeAvgCount - 1]) <
                         this->GetFrameMSecByFixedMode();

        // 更新時間より早い
        return bWait;
    }

    HE::Bool FPS::UpdateTime(Core::Memory::WeakPtr<Platform::TimeInterface> in_wpTimeInterface)
    {
        // ミリ秒単位で扱っている
        auto pTime = in_wpTimeInterface.lock();
        if (pTime == NULL) return FALSE;

        const HE::Uint64 uNowMSec = pTime->VNowMSec();
        // 最新の時間と前フレーム前の時間との差
        const HE::Uint64 uFrameTimeMSec = uNowMSec - this->_uaPreviousTimeMSec[0];

        // 格納した時間をずらす
        HE::Uint32 _uSize =
            HE_ARRAY_SIZE(this->_uaPreviousTimeMSec) - sizeof(this->_uaPreviousTimeMSec[0]);
        ::memmove(this->_uaPreviousTimeMSec, &this->_uaPreviousTimeMSec[1], _uSize);

        // フレームレートを計算
        {
            // 0除算を回避
            if (0 < uFrameTimeMSec)
            {
                // 1秒 / フレーム更新したミリ秒時間 = フレーム更新したレート値
                this->_uFrameRate = static_cast<HE::Uint32>(
                    FPS::_ffFrameAvgTimeMSec / static_cast<HE::Float64>(uFrameTimeMSec));
            }
            else
            {
                this->_uFrameRate = 0.0f;
            }
        }

        // 更新
        this->_uaPreviousTimeMSec[FPS::_uTimeAvgCount - 1] = uNowMSec;

        return FALSE;
    }

    HE::Float32 FPS::GetDeltaTimeMSec() const
    {
        return (1.0f / this->_uFrameRate) * 1000.0f;
    }

    HE::Float32 FPS::GetDeltaTimeSec() const
    {
        HE::Float32 fDeletaTime = this->GetDeltaTimeMSec();
        if (fDeletaTime <= 0.0f) return 0.0f;

        // 秒に変換
        fDeletaTime /= 1000.0f;

        return fDeletaTime;
    }
}  // namespace Core::Time
