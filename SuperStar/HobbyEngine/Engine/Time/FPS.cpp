#include "FPS.h"

#include "Engine/Platform/PlatformModule.h"

namespace Core::Time
{
    FPS::FPS(Platform::TimeInterface* in_pTimeInterface)
    {
        // 帰ってくる時間の単位はmsec
        auto uCurrentTime = in_pTimeInterface->VNowMSec();
        for (Uint32 i = 0; i < FPS::_uTimeAvgCount; ++i)
            this->_uaPreviousTimeMSec[i] = uCurrentTime;
    }

    Bool FPS::UpdateWait(Platform::TimeInterface* in_pTimeInterface, const Uint32 in_uWaitMSec)
    {
        HE_ASSERT(in_pTimeInterface);
        // ミリ秒単位で扱っている

        const Uint64 uNowMSec = in_pTimeInterface->VNowMSec();
        // 最新の時間と前フレーム前の時間との差
        const Uint64 uFrameTimeMSec = uNowMSec - this->_uaPreviousTimeMSec[0];

        // Waitタイムより早い場合は待機フラグを返す
        Bool bWait = (uNowMSec - this->_uaPreviousTimeMSec[FPS::_uTimeAvgCount - 1]) < in_uWaitMSec;

        // 更新時間より早い
        if (bWait) return TRUE;

        // 格納した時間をずらす
        Uint32 uSize =
            HE_ARRAY_SIZE(this->_uaPreviousTimeMSec) - sizeof(this->_uaPreviousTimeMSec[0]);
        ::memmove(this->_uaPreviousTimeMSec, &this->_uaPreviousTimeMSec[1], uSize);

        // フレームレートを計算
        {
            // 0除算を回避
            if (0 < uFrameTimeMSec)
            {
                // 1秒 / フレーム更新したミリ秒時間 = フレーム更新したレート値
                this->_uFrameRate = FPS::_fFrameAvgTimeMSec / static_cast<Float32>(uFrameTimeMSec);
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

    Float32 FPS::GetDeltaTimeMSec() const
    {
        return (1.0f / this->_uFrameRate) * 1000.0f;
    }

    Float32 FPS::GetDeltaTimeSec() const
    {
        Float32 fDeletaTime = this->GetDeltaTimeMSec();
        if (fDeletaTime <= 0.0f) return 0.0f;

        // 秒に変換
        fDeletaTime /= 1000.0f;

        return fDeletaTime;
    }
}  // namespace Core::Time
