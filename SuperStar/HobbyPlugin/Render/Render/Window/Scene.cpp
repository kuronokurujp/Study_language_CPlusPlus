#include "Scene.h"

#include "Engine/Platform/PlatformModule.h"

namespace Render
{
    SceneViewBase::SceneViewBase()
    {
        this->_runtimePoolPrticleBlob.ReservePool(1024);
    }

    HE::Bool SceneViewBase::_Begin(
        Core::Memory::UniquePtr<Platform::SceneStrategyInterface> in_upSt)
    {
        this->_upSt = std::move(in_upSt);
        return TRUE;
    }

    void SceneViewBase::Release()
    {
        this->_runtimePoolPrticleBlob.ReleasePool();
        HE_SAFE_DELETE_UNIQUE_PTR(this->_upSt);
    }

    const Render::Prticle::Blob& SceneViewBase::GetPrticle(const Core::Common::Handle in_handle)
    {
        // TODO: return ステートメントをここに挿入します
        return *(this->_runtimePoolPrticleBlob.Ref(in_handle));
    }

    void SceneViewBase::_End()
    {
    }

    void SceneViewBase::_Update(const HE::Float32)
    {
    }

    void SceneViewBase::_BeginRender()
    {
        this->_upSt->VBeginRender();
    }

    void SceneViewBase::_EndRender()
    {
        this->_upSt->VEndRender();
    }

    HE::Bool SceneViewBase::_PushCommand(Command&& in_rrCmd)
    {
        // コマンドをコピー
        // 要素を先頭に追加
        // 最後に追加したコマンドは配列先頭に
        // 最初に追加したコマンドは配列末尾に
        // TODO: コマンドをまとめる
        // テキスト描画コマンドなど描画コマンドはまとめるようにする
        // 描画処理をまとめる事で高速処理をする
        switch (in_rrCmd._uType)
        {
            // 画面を指定色でクリア
            case ECmdType_ClsScreen:
                break;
            // テキスト描画
            case ECmdType_2DTextDraw:
                break;
            // 矩形描画
            case ECmdType_2DQuadDraw:
                break;
                /*
            // 点描画
            case ECmdType_2DPointDraw:
                break;
            // 点群描画
            case ECmdType_2DPointArrayDraw:
                break;
                */
            // 円の描画
            case ECmdType_2DCircleDraw:
                break;
            // 三角形の描画
            case ECmdType_2DTriangleDraw:
                break;
        }
        this->_commandBuff.PushFront(in_rrCmd);

        return TRUE;
    }
}  // namespace Render
