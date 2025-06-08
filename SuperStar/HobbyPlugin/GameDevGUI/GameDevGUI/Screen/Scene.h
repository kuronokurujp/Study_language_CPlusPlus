#pragma once

#include "Engine/MiniEngine.h"
#include "Engine/Platform/PlatformScreen.h"

namespace GameDevGUI
{
    /// <summary>
    /// ImGUI用のシーン
    /// 継承して利用してコード上でGUIを形成する
    /// </summary>
    class GameDevGUISceneStrategy : public Platform::SceneStrategyInterface
    {
    public:
        GameDevGUISceneStrategy(Core::Memory::UniquePtr<Platform::SceneStrategyInterface>);

        HE::Bool VBegin() override;
        void VEnd() override final;

        void VUpdate(const HE::Float32) override final;

        void VBeginRender() override final;
        void VEndRender() override final;

        // TODO: ボタンやテキストを扱うメソッドを作る
        // TODO: タイトル名を入れる

        /// <summary>
        /// シーンを描画するプラットフォームのインスタンス
        /// </summary>
        /// <returns></returns>
        virtual Platform::RenderInterface* VGetRenderer() override final { return NULL; }

    private:
        Core::Memory::UniquePtr<Platform::SceneStrategyInterface> _upSt;
    };

}  // namespace GameDevGUI
