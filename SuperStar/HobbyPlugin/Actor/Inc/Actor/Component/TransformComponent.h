#pragma once

#include "Actor/Component/Component.h"
#include "Engine/Math/Math.h"

// エンジン最小インクルード
#include "Engine/MiniEngine.h"

namespace Actor
{
    /// <summary>
    /// 座標や回転,スケールなどを扱うコンポーネント
    /// </summary>
    class TransformComponent final : public Component
    {
        HE_CLASS_COPY_NG(TransformComponent);
        HE_GENERATED_CLASS_BODY_HEADER(TransformComponent, Component);

    public:
        TransformComponent() : Component() { this->_Clear(); }

        /// <summary>
        /// 親の位置を含めたワールド座標取得
        /// </summary>
        const Core::Math::Vector3 GetWorldPos();

        inline const Core::Math::Vector3& GetDir() const { return this->_dir; }

        /// <summary>
        /// 座標設定
        /// </summary>
        inline void SetPos(const Core::Math::Vector3& in_rPosition) { this->_pos = in_rPosition; }

        /// <summary>
        /// 方向設定
        /// </summary>
        inline void SetDir(const Core::Math::Vector3& in_rDirection) { this->_dir = in_rDirection; }

        // 座標変換一覧
        void TransformLocalToWorldPos2D(Core::Math::Vector2* out,
                                        const Core::Math::Vector2& in_rOffsetPos);
        void TransformLocalToWorldRect2D(Core::Math::RC::Rect2D* out,
                                         const Core::Math::RC::Rect2D& in_rOffsetRect);

        /// <summary>
        /// 座標や回転,スケールを加えた最新のワールド行列を作る
        /// 最新のワールド行列がほしい場合は呼び出す必要がある
        /// </summary>
        const Core::Math::Matrix4& ComputeWorldTransform();

    private:
        void _Clear()
        {
            this->_pos.Clear();
            this->_dir.Clear();
            this->_scale.Clear();
        }

    private:
        Core::Math::Vector3 _pos;
        Core::Math::Vector3 _dir;
        Core::Math::Vector3 _scale;
        Core::Math::Quaternion _rotation;

        Core::Math::Matrix4 _worldTransform;
    };
}  // namespace Actor
