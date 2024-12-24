#pragma once

#include <functional>

#include "Engine/Common/CustomVector.h"
#include "Engine/Math/Vector3.h"
#include "Engine/MiniEngine.h"

// 利用モジュール
#include "ActorModule.h"

namespace InGame
{
    enum ECollisionType
    {

        ECollisionType_None = 0,
        ECollisionType_Circle2D,
    };

    struct CollisionData
    {
        ECollisionType eType;
        HE::Uint32 uHashCode = 0;

        // メタデータ
        // 利用者が自由に使えるもの
        HE::Uint64 ulMetaData = 0;

        union
        {
            HE::Uint8 aWork[256] = {0};

            struct Circle2D
            {
                Core::Math::Vector2 pos;
                HE::Float32 fRadius;
            } circle2D;

        } _data;
    };

    /// <summary>
    /// インゲームのアクター衝突コンポーネント
    /// </summary>
    class InGameCollisionComponent : public Actor::Component
    {
        HE_CLASS_COPY_NG(InGameCollisionComponent);
        HE_CLASS_MOVE_NG(InGameCollisionComponent);
        HE_GENERATED_CLASS_BODY_HEADER(InGameCollisionComponent, Actor::Component);

    public:
        InGameCollisionComponent()          = default;
        virtual ~InGameCollisionComponent() = default;

        /// <summary>
        /// タスク開始
        /// </summary>
        virtual HE::Bool VBegin() override;

        /// <summary>
        /// タスク終了
        /// </summary>
        virtual HE::Bool VEnd() override;

        virtual HE::Uint32 VColCount() const                                               = 0;
        virtual HE::Bool VOutputColData(CollisionData* out, const HE::Uint32 in_uColIndex) = 0;

        /// <summary>
        /// 衝突発生
        /// </summary>
        virtual HE::Bool VOnHit(const CollisionData& in_rSelfColData,
                                const CollisionData& in_rHitColData);

        void SetCollisionHashCode(const HE::Char* in_szName);
        void SetCollisionHashCode(const HE::Uint32 in_uHashCode);
        void SetMetaData(const HE::Uint64 in_ulMetaData);

        HE::Uint32 HashCode() const { return this->_uHashCode; }

        void SetHitAction(
            std::function<HE::Bool(const CollisionData&, const CollisionData&)> in_func)
        {
            this->_hitAction = in_func;
        }

    protected:
        HE::Uint32 _uHashCode  = 0;
        HE::Uint64 _ulMetaData = 0;

    private:
        std::function<HE::Bool(const CollisionData&, const CollisionData&)> _hitAction = NULL;
    };

    class InGameCircleCollision2DComponent final : public InGameCollisionComponent
    {
        HE_CLASS_COPY_NG(InGameCircleCollision2DComponent);
        HE_CLASS_MOVE_NG(InGameCircleCollision2DComponent);
        HE_GENERATED_CLASS_BODY_HEADER(InGameCircleCollision2DComponent, InGameCollisionComponent);

    public:
        InGameCircleCollision2DComponent() = default;

        // 半径値
        void SetRadius(const HE::Float32 in_fRadius) { this->_fRadius = in_fRadius; }

        HE::Uint32 VColCount() const override final { return 1; }
        HE::Bool VOutputColData(CollisionData* out, const HE::Uint32 in_uColIndex) override final;

    private:
        HE::Float32 _fRadius = 0.0f;
    };

    // 全ての衝突コンポーネント同士の衝突処理
    extern void CollisionAll();

}  // namespace InGame
