#pragma once

#include "Engine/Math/Vector2.h"

// エンジン最小インクルード
#include "Engine/MiniEngine.h"
#include "InGameBulletInterface.h"

namespace InGame
{
    struct InGameBulletNormalWork
    {
        // 弾の位置
        Core::Math::Vector2 pos;

        // 弾の移動方向
        Core::Math::Vector2 dir;

        HE::Float32 fFirstSpeed = 6.0f;
        HE::Float32 fSpeed      = 0.0f;
        HE::Float32 fAccele     = 0.0f;

        // 弾のサイズ
        HE::Float32 _fSize = 32.0f;
    };

    /// <summary>
    /// 通常弾の生成
    /// </summary>
    class InGameBulletNormalFactory final : public InGameBulletFactoryInterface
    {
        HE_CLASS_COPY_NG(InGameBulletNormalFactory);

    public:
        InGameBulletNormalFactory(Core::Math::Vector2& in_rPos, Core::Math::Vector2& in_rDir,
                                  const HE::Uint32 in_uCollisionHashCode)
            : _pos(in_rPos), _dir(in_rDir), _uCollisionHashCode(in_uCollisionHashCode)
        {
        }

        const HE::Char* VName() const;
        void VConfiguration(InGameBulletWork* in_pWork);

    private:
        Core::Math::Vector2 _pos;
        Core::Math::Vector2 _dir;
        HE::Uint32 _uCollisionHashCode = 0;
    };

    /// <summary>
    /// 通常弾の制御アルゴリズム
    /// </summary>
    class InGameBulletNormalStrategy final : public InGameBulletStrategyInterface
    {
        HE_CLASS_COPY_NG(InGameBulletNormalStrategy);

    public:
        //	コンストラクタ
        InGameBulletNormalStrategy() = default;

        HE::Uint32 VWorkSize() const override final;
        const HE::Char* VName() const override final;

        HE::Bool VUpdate(InGameBulletWork* in_pWork,
                         Core::Common::Handle& in_rViewHandle) override final;

        HE::Bool VIsScreenInSide(const InGameBulletWork* const,
                                 const Core::Math::Rect2&) override final;

        HE::Bool VOutputCollisionData(CollisionData* out,
                                      const InGameBulletWork* const) override final;
    };
}  // namespace InGame
