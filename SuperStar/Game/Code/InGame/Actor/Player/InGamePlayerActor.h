#pragma once

#include "Engine/Common/CustomMap.h"
#include "InGame/Actor/InGameScene2DActor.h"
#include "InGame/Shot/InGameShotStrategyInterface.h"

// エンジンの最小インクルード
#include "Engine/MiniEngine.h"

namespace InGame
{
    class InGamePlayerActor final : public InGameScene2DActor
    {
        HE_GENERATED_CLASS_BODY_HEADER(InGamePlayerActor, InGameScene2DActor);

    public:
        /// <summary>
        /// 自機のパラメータ
        /// </summary>
        struct Parameter
        {
            // 移動スピード
            HE::Float32 speed = 5.0f;

            // 無敵時間
            HE::Float32 fInvincibleTimeSec = 1.5;
            // HP
            HE::Sint32 ulife = 3;
        };

    public:
        InGamePlayerActor(const Parameter& in_defaultParam);
        virtual ~InGamePlayerActor() = default;

        /// <summary>
        /// 開始
        /// 継承したクラスで必ず基本クラスのメソッドを呼ぶ
        /// </summary>
        HE::Bool VBegin() override final;

        /// <summary>
        /// 終了
        /// 継承したクラスで必ず基本クラスのメソッドを呼ぶ
        /// </summary>
        HE::Bool VEnd() override final;

        void VUpdate(const HE::Float32 in_fDt) override final;

        void SetSize(const Core::Math::Vector2&);

        void Move(const Core::Math::Vector2&);
        void Shot();
        HE::Bool Damage(const HE::Sint32 in_sDamage);

        const Parameter& GetParameter() const { return this->_parameter; }

#if 0
          //	撃てる弾の名前を取得
        const char* GetActiveShotName() const;
        const char* GetShotName(const int in_Type) const;
        int GetShotTypeNum() const { return eSHOT_MAX; }

        int GetLifeNum() const { return m_life; }

        void MoveShotType(const int in_MoveNum);

        //	データ通知
        virtual void VOnCallbackSendMessage(const int in_DataType, void* in_pData);
#endif

    private:
        void _Clear()
        {
            this->_size.Clear();
            this->_move.Clear();
        }

    private:
        Core::Math::Vector2 _size;
        Core::Math::Vector2 _move;

        Core::Common::Handle _collisoinHandle;
        Core::Common::Handle _shotHandle;

        // ランタイム中に変化するパラメータ
        Parameter _parameter;
        // 初期パラメータ
        Parameter _defaultParameter;

        Core::Common::FixedMap<Core::Common::FixedString32,
                               Core::Memory::SharedPtr<InGameShotStrategyInterface>, 6>
            _mShotStrategy;
    };

}  // namespace InGame
