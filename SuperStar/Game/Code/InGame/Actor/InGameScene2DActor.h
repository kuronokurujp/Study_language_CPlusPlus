#pragma once

// エンジンの最小インクルード
#include "Engine/MiniEngine.h"

// 利用モジュール
#include "ActorModule.h"

namespace InGame
{
    class InGameScene2DActor : public Actor::Object
    {
        HE_GENERATED_CLASS_BODY_HEADER(InGameScene2DActor, Actor::Object);

    public:
        InGameScene2DActor();
        virtual ~InGameScene2DActor() = default;

        /// <summary>
        /// 開始
        /// 継承したクラスで必ず基本クラスのメソッドを呼ぶ
        /// </summary>
        HE::Bool VBegin() override;

        void SetPos(const Core::Math::Vector2&);
        void SetDir(const Core::Math::Vector2&);

    private:
        void _Clear()
        {
            this->_pos.Clear();
            this->_dir.Clear();
            this->_transformHandle.Clear();
        }

    protected:
        Core::Math::Vector2 _pos, _dir;
        Core::Common::Handle _transformHandle;
    };

}  // namespace InGame
