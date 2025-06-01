#pragma once

#include <functional>

#include "Engine/Common/CustomMap.h"

// エンジンの最小インクルード
#include "Engine/MiniEngine.h"
#include "InGame/Bullet/InGameBulletInterface.h"
#include "InGame/Component/InGameCollisionComponent.h"

// イベントモジュールを使う
#include "EventModule.h"

namespace InGame
{
    /// <summary>
    /// 味方や敵の弾を管理するコンポーネント
    /// </summary>
    class InGameBulletManagerComponent final : public InGameCollisionComponent
    {
        HE_CLASS_COPY_NG(InGameBulletManagerComponent);
        HE_CLASS_MOVE_NG(InGameBulletManagerComponent);
        HE_GENERATED_CLASS_BODY_HEADER(InGameBulletManagerComponent,
                                       InGame::InGameCollisionComponent);

    private:
        class WorkFixVector final
            : public Core::Common::FixedVector<InGame::InGameBulletObject, 256>
        {
        public:
            void RemoveAt(const HE::Uint32 in_uIndex) { this->_RemoveAt(in_uIndex); }
        };

    public:
        explicit InGameBulletManagerComponent(Core::Common::Handle in_rViewHandle);

        /// <summary>
        /// 初期化
        /// </summary>
        void VSetup(const HE::Bool in_bReleaseMem) override final;

        /// <summary>
        /// コンポーネント開始
        /// </summary>
        HE::Bool VBegin() override final;

        /// <summary>
        /// コンポーネント終了
        /// </summary>
        HE::Bool VEnd() override final;

        /// <summary>
        /// コンポーネントの更新
        /// </summary>
        void VUpdate(const HE::Float32 in_fDt) override final;

        /// <summary>
        /// 衝突発生
        /// </summary>
        HE::Bool VOnHit(const CollisionData& in_rSelfColData,
                        const CollisionData& in_rHitColData) override final;

        /// <summary>
        /// 弾の生成
        /// </summary>
        HE::Bool MakeObject(Core::Memory::UniquePtr<InGameBulletFactoryInterface>);

        /// <summary>
        /// 弾のアルゴリズムを追加
        /// </summary>
        HE::Bool AddStrategy(Core::Memory::UniquePtr<InGameBulletStrategyInterface>);

        // コリジョン処理
        HE::Uint32 VColCount() const override final { return this->_vBullet.Size(); }
        HE::Bool VOutputColData(CollisionData* out, const HE::Uint32 in_uColIndex) override final;

    private:
        void _ForEachObject(std::function<HE::Bool(InGame::InGameBulletObject*,
                                                   InGame::InGameBulletStrategyInterface*)>);

        HE::Bool _HandleEvent(Event::EventDataInterfacePtr const&);

    private:
        Core::Common::FixedMap<Core::Common::FixedString128,
                               Core::Memory::UniquePtr<InGame::InGameBulletStrategyInterface>, 32>
            _mBulletStrategy;

        WorkFixVector _vBullet;
        Core::Common::Handle _viewHandle;
        Core::Common::Handle _eventListeningHandle;
    };
}  // namespace InGame
