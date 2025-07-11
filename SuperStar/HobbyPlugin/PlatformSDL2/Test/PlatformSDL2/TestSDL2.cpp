#include <gtest/gtest.h>

#include "Engine/Engine.h"
#include "PlatformSDL2/Graphic/Drawable2D.h"
#include "PlatformSDL2/SDL2File.h"
#include "PlatformSDL2/SDL2System.h"
#include "PlatformSDL2/SDL2Time.h"
#include "PlatformSDL2Module.h"

// GUIテストをしている
// CIでテストする時には一定時間経過したらGUIを閉じる仕組みが必要
// OpenGLなので座標系は右手座標系で座標設定
// また原点は画面の中心
namespace PlatformSDL2
{
    // TODO: モジュールテストとフォントや画面の表示テストを分けるべき
    /// <summary>
    /// フォント表示のテスト
    /// </summary>
    TEST(HobbyPlugin_PlatformSDL2, FontDraw)
    {
        Core::Memory::SharedPtr<PlatformSDL2::PlatformSDL2Module> spSDL2Module =
            HE_MAKE_CUSTOM_SHARED_PTR((PlatformSDL2::PlatformSDL2Module));

        Module::ModuleManager moduleManager;
        EXPECT_EQ(moduleManager.AddModule(spSDL2Module), TRUE);

        EXPECT_EQ(moduleManager.Start(), TRUE);

        // フォントデータのバイナリアセットを作成
        auto spPlatformModule = moduleManager.Get<Platform::PlatformModule>();
        {
            // ロードするフォントファイルパスを渡す
            // シェーダーファイルも渡す.(プラットフォームによっては使わない)
            auto bRet =
                spPlatformModule->Font()->VLoad(Platform::EFontSize_64,
                                                HE_STR_TEXT("Resources/SDL2/Font/TestFont.ttf"));
            EXPECT_EQ(bRet, TRUE);
        }

        // 入力オブジェクト作成
        auto inputHandle = spPlatformModule->Input()->VCreateObject();

        // フレームイベント
        class TestEvent : public Platform::EventFrameInterface
        {
        public:
            void VDraw(Platform::MapDrawable& in_mDrawble) override final
            {
                // 利用する描画を取得
                // 事前に描画は登録されている
                EXPECT_EQ(in_mDrawble.Contains(PlatformSDL2::Drawable2D::StaticRTTI()), TRUE);

                // 利用する描画型のキャスト
                auto pDrawable = reinterpret_cast<PlatformSDL2::Drawable2D*>(
                    in_mDrawble.FindKey(PlatformSDL2::Drawable2D::StaticRTTI())->_data.get());

                // 描画前処理
                pDrawable->VPreDraw();
                {
                    // テキスト表示
                    {
                        auto screenLeftTopPos = Core::Math::Vector2(-320.f, 240.0f);
                        pDrawable->Draw2DText(screenLeftTopPos, Core::Math::EAnchor::EAnchor_Left,
                                              HE_STR_TEXT("Test"), 32, 1, Core::Math::RGB::White);
                    }

                    // テキスト表示
                    {
                        Core::Common::FixedString1024 s3(HE_STR_TEXT("AABBBBBBBAABBBBBBBAA"));
                        pDrawable->Draw2DText(Core::Math::Vector2(.0f, .0f),
                                              Core::Math::EAnchor_Center, s3.Str(), 32, 1.f,
                                              Core::Math::RGB::Blue);
                    }
                }
                // 描画後処理
                pDrawable->VPostDraw();
            }
        };

        // フレーム作成
        Core::Common::Handle frameHandle;

        const auto uW = 640;
        const auto uH = 480;
        {
            // フレーム設定
            // フレームサイズ、入力ハンドル
            auto windowConfig = Platform::FrameConfig(uW, uH, 1, TRUE, inputHandle,
                                                      Platform::FrameConfig::EFlags_None);

            // 設定に基づいたフレーム生成
            auto spGraphicModule = spPlatformModule->Graphic();
            frameHandle =
                spGraphicModule->VCreateFrame(windowConfig, HE_MAKE_CUSTOM_UNIQUE_PTR((TestEvent)));
        }

        // 描画作成
        {
            auto spGraphicModule = spPlatformModule->Graphic();
            // SDL2の2D描画と追加
            auto upDrawable2D = HE_MAKE_CUSTOM_UNIQUE_PTR((PlatformSDL2::Drawable2D));

            // 描画に必要な設定をする
            upDrawable2D->VSetViewSize(uW, uH);
            upDrawable2D->SetFont(spPlatformModule->Font());

            spGraphicModule->VAddDrawable(std::move(upDrawable2D));
        }

        // フレームの表示
        {
            auto spGraphicModule = spPlatformModule->Graphic();
            auto pFrame          = spGraphicModule->VGetFrame(frameHandle);
            pFrame->VShow();
        }

        // フォント表示
        while (spPlatformModule->VIsQuit() == FALSE)
        {
            moduleManager.BeforeUpdate(0);
            moduleManager.Update(0);
            moduleManager.LateUpdate(0);
        }

        EXPECT_EQ(moduleManager.Release(), TRUE);
    }
#if 0
// TODO: 右手座標系と矩形の起点位置を修正したので矩形表示位置がずれているので修正する
TEST(HobbyPlugin_PlatformSDL2, QuadDraw)
{
    Core::Memory::SharedPtr<PlatformSDL2::PlatformSDL2Module> spSDL2Module =
        HE_MAKE_CUSTOM_SHARED_PTR((PlatformSDL2::PlatformSDL2Module));

    Module::ModuleManager moduleManager;
    EXPECT_EQ(moduleManager.AddModule(spSDL2Module), TRUE);

    EXPECT_EQ(moduleManager.Start(), TRUE);

    // フォントデータのバイナリアセットを作成
    auto spPlatformModule = moduleManager.Get<Platform::PlatformModule>();
    {
        // ロードするフォントファイルパスを渡す
        // シェーダーファイルも渡す.(プラットフォームによっては使わない)
        auto bRet =
            spPlatformModule->Font()->VLoad(Platform::EFontSize_64,
                                            HE_STR_TEXT("Resources/SDL2/Font/TestFont.ttf"));
        EXPECT_EQ(bRet, TRUE);
    }

    // ウィンドウ追加
    {
        auto inputHandle = spPlatformModule->Input()->VCreateObject();
        // ウィンドウの設定
        auto windowConfig =
            Platform::WindowConfig{640,  480,         1,
                                   TRUE, inputHandle, Platform::WindowConfig::EFlags_None};

        // 設定に基づいたウィンドウを生成
        auto handle  = spPlatformModule->Graphic()->VCreateWindowStrategy(windowConfig);
        auto pWindow = spPlatformModule->Graphic()->VGetWindow(handle);
        // ビューポートの追加
        auto viewPortConfig = Platform::ViewPortConfig{640, 480};
        auto viewPortHandle = pWindow->CreateViewPort(viewPortConfig);
        auto pViewPort      = pWindow->GetViewPort(viewPortHandle);
        // 描画するシーンの追加
        auto sceneHandle = pViewPort->CreateScene<PlatformSDL2::DefaultRender>(
            [spPlatformModule](Core::Memory::SharedPtr<Platform::RenderInterface> in_spRender,
                               const Platform::SceneConfig& in_rSceneConfig)
            {
                // シーンの渡したレンダリングを利用して描画する
                HE_ASSERT_RETURN(
                    HE_GENERATED_CHECK_RTTI(*in_spRender, PlatformSDL2::DefaultRender));
                auto pDrawable = HE_SHADER_PTR_CAST(PlatformSDL2::DefaultRender, in_spRender);
                {
                    // 画面左上の座標
                    auto screenLeftTopPos = Core::Math::Vector2(-in_rSceneConfig.GetWidthHalf(),
                                                                in_rSceneConfig.GetHeightHalf());
                    // ウィンドウが閉じたら終了
                    Core::Math::RC::Rect2D rect(screenLeftTopPos._fX, screenLeftTopPos._fY, 32.f,
                                                32.0f);
                    // 左隅にぴったり表示しているかのテスト
                    {
                        pDrawable->Draw2DQuad(in_rSceneConfig, rect, Core::Math::EAnchor_Left,
                                            Core::Math::RGB::Red);
                    }

                    // 左隅の矩形と1ドット右にずらして矩形表示しているかテスト
                    {
                        rect.Set(screenLeftTopPos._fX + 33.f, screenLeftTopPos._fY, 32.0f, 32.0f);
                        pDrawable->Draw2DQuad(in_rSceneConfig, rect, Core::Math::EAnchor_Left,
                                            Core::Math::RGB::Yellow);
                    }

                    // 左隅の矩形と1ドット下にずらして矩形表示しているかテスト
                    {
                        rect.Set(screenLeftTopPos._fX, screenLeftTopPos._fY - 33.0f, 32.0f, 32.0f);
                        pDrawable->Draw2DQuad(in_rSceneConfig, rect, Core::Math::EAnchor_Left,
                                            Core::Math::RGB::Yellow);
                    }
                    // テキストの下地で矩形が表示しているかテスト
                    {
                        Core::Common::FixedString1024 s2(
                            HE_STR_TEXT("タイトルルルルルルルルルルルルルルるあ"));

                        rect.Set(screenLeftTopPos._fX, screenLeftTopPos._fY - (32.0f * 5.0f),
                                 (32.0f * s2.Length()) + (2.0f * (s2.Length() - 1)), 32.0f);
                        pDrawable->Draw2DQuad(in_rSceneConfig, rect, Core::Math::EAnchor_Left,
                                            Core::Math::RGB::Red);

                        // テキスト表示
                        pDrawable->Draw2DText(in_rSceneConfig, spPlatformModule->Font().get(),
                                            Core::Math::Vector2(screenLeftTopPos._fX,
                                                                screenLeftTopPos._fY - 32.0f * 5.f),
                                            Core::Math::EAnchor_Left, s2.Str(), 32, 2.0f,
                                            Core::Math::RGB::Blue);
                    }

                    // テキストの下地で矩形が表示しているかテスト
                    {
                        Core::Common::FixedString1024 s3(HE_STR_TEXT("MMM"));
                        rect.Set(screenLeftTopPos._fX, screenLeftTopPos._fY - (64.0f * 5.0f),
                                 (16.0f * s3.Length()) + (s3.Length() - 1), 16.0f);
                        pDrawable->Draw2DQuad(in_rSceneConfig, rect, Core::Math::EAnchor_Left,
                                            Core::Math::RGB::Red);

                        pDrawable->Draw2DText(in_rSceneConfig, spPlatformModule->Font().get(),
                                            Core::Math::Vector2(screenLeftTopPos._fX,
                                                                screenLeftTopPos._fY - 64.0f * 5.f),
                                            Core::Math::EAnchor_Left, s3.Str(), 16, 1.0f,
                                            Core::Math::RGB::Blue);
                    }
                    // 画面真ん中に矩形表示のアンカーが真ん中
                    {
                        rect.Set(.0f, .0f, 32.0f, 32.0f);
                        pDrawable->Draw2DQuad(in_rSceneConfig, rect, Core::Math::EAnchor_Center,
                                            Core::Math::RGB::Red);
                    }

                    // 画面真ん中に矩形表示してかつアンカーが左上
                    {
                        rect.Set(.0f, .0f, 32.0f, 32.0f);
                        pDrawable->Draw2DQuad(in_rSceneConfig, rect, Core::Math::EAnchor_Left,
                                            Core::Math::RGB::White);
                    }

                    // 画面左下に矩形表示してかつアンカーが左上
                    {
                        rect.Set(screenLeftTopPos._fX, -in_rSceneConfig.GetHeightHalf() + 32.0f,
                                 32.0f, 32.0f);
                        pDrawable->Draw2DQuad(in_rSceneConfig, rect, Core::Math::EAnchor_Left,
                                            Core::Math::RGB::White);
                    }

                    // 画面右上に矩形表示してかつアンカーが左上
                    {
                        rect.Set(in_rSceneConfig.GetWidthHalf() - 32.0f,
                                 in_rSceneConfig.GetHeightHalf(), 32.0f, 32.0f);
                        pDrawable->Draw2DQuad(in_rSceneConfig, rect, Core::Math::EAnchor_Left,
                                            Core::Math::RGB::White);
                    }

                    // 画面右下に矩形表示してかつアンカーが左上
                    {
                        rect.Set(in_rSceneConfig.GetWidthHalf() - 32.0f,
                                 -in_rSceneConfig.GetHeightHalf() + 32.0f, 32.0f, 32.0f);
                        pDrawable->Draw2DQuad(in_rSceneConfig, rect, Core::Math::EAnchor_Left,
                                            Core::Math::RGB::White);
                    }

                    // アンカーが左上で円描画の画面真ん中上のテスト
                    {
                        Core::Math::Vector2 pos(.0f, .0f);
                        pDrawable->Draw2DCircle(in_rSceneConfig, pos, Core::Math::EAnchor_Left, 32,
                                              Core::Math::RGB::White);
                    }

                    // アンカーが中心で画面真ん中下の円描画のテスト
                    {
                        Core::Math::Vector2 pos(.0f, .0f);
                        pDrawable->Draw2DCircle(in_rSceneConfig, pos, Core::Math::EAnchor_Center, 32,
                                              Core::Math::RGB::White);
                    }

                    // アンカー中心三角形描画テスト
                    {
                        Core::Math::Vector2 pos(.0f, 0.f);
                        pDrawable->Draw2DTriangle(in_rSceneConfig, pos, Core::Math::EAnchor_Center,
                                                0.0f, 32.0f, Core::Math::RGB::Blue);
                    }

                    // アンカー左上の三角形描画テスト
                    {
                        Core::Math::Vector2 pos(200.f, 0.f);
                        pDrawable->Draw2DTriangle(in_rSceneConfig, pos, Core::Math::EAnchor_Left,
                                                -90.0f, 32.0f, Core::Math::RGB::White);
                    }
                }
            });

        pWindow->VShow();
    }

    // 描画
    while (spPlatformModule->VIsQuit() == FALSE)
    {
        moduleManager.BeforeUpdate(0);
        moduleManager.Update(0);
        moduleManager.LateUpdate(0);
    }

    EXPECT_EQ(moduleManager.Release(), TRUE);
}

TEST(HobbyPlugin_PlatformSDL2, ParticleDraw)
{
    Core::Memory::SharedPtr<PlatformSDL2::PlatformSDL2Module> spSDL2Module =
        HE_MAKE_CUSTOM_SHARED_PTR((PlatformSDL2::PlatformSDL2Module));

    Module::ModuleManager moduleManager;
    EXPECT_EQ(moduleManager.AddModule(spSDL2Module), TRUE);

    EXPECT_EQ(moduleManager.Start(), TRUE);

    // フォントデータのバイナリアセットを作成
    auto spPlatformModule = moduleManager.Get<Platform::PlatformModule>();
    {
        // ロードするフォントファイルパスを渡す
        // シェーダーファイルも渡す.(プラットフォームによっては使わない)
        auto bRet =
            spPlatformModule->Font()->VLoad(Platform::EFontSize_64,
                                            HE_STR_TEXT("Resources/SDL2/Font/TestFont.ttf"));
        EXPECT_EQ(bRet, TRUE);
    }

    // ウィンドウ追加
    {
        auto inputHandle = spPlatformModule->Input()->VCreateObject();
        // ウィンドウの設定
        auto windowConfig =
            Platform::WindowConfig{640,  480,         1,
                                   TRUE, inputHandle, Platform::WindowConfig::EFlags_None};

        // 設定に基づいたウィンドウを生成
        auto handle  = spPlatformModule->Graphic()->VCreateWindowStrategy(windowConfig);
        auto pWindow = spPlatformModule->Graphic()->VGetWindow(handle);
        // ビューポートの追加
        auto viewPortConfig = Platform::ViewPortConfig{640, 480};
        auto viewPortHandle = pWindow->CreateViewPort(viewPortConfig);
        auto pViewPort      = pWindow->GetViewPort(viewPortHandle);

        Core::Math::Vector3 pos(.0f, .0f, 0.0f);
        Core::Common::Handle particleObjHandle;
        // 描画するシーンの追加
        auto sceneHandle = pViewPort->CreateScene<PlatformSDL2::DefaultRender>(
            [spPlatformModule, &particleObjHandle,
             &pos](Core::Memory::SharedPtr<Platform::RenderInterface> in_spRender,
                   const Platform::SceneConfig& in_rSceneConfig)
            {
                // シーンの渡したレンダリングを利用して描画する
                HE_ASSERT_RETURN(
                    HE_GENERATED_CHECK_RTTI(*in_spRender, PlatformSDL2::DefaultRender));
                auto pDrawable = HE_SHADER_PTR_CAST(PlatformSDL2::DefaultRender, in_spRender);
                // 点群表示
                pDrawable->Draw2DPartical(in_rSceneConfig, particleObjHandle, pos);
                // 点群が上に移動
                pos._fY += 0.5f;
            });
        {
            auto spScene         = pViewPort->GetScene(sceneHandle);
            auto spRender        = spScene->GetRender();
            auto spDefaultRender = HE_SHADER_PTR_CAST(PlatformSDL2::DefaultRender, spRender);

            // パーティクルオブジェクト作成
            particleObjHandle = spDefaultRender->CreateParticleObject(1000);

            // 各パーティクル位置と速度と色のデータ作成
            Core::Common::FixedArray<Core::Math::Vector3, 1000> aPos;
            Core::Common::FixedArray<Core::Math::Vector3, 1000> aVelocity;
            Core::Common::FixedArray<Core::Math::Color, 1000> aColor;
            {
                for (HE::Uint32 i = 0; i < 1000; ++i)
                {
                    auto x = spPlatformModule->System()->VGetRandByFloat(-1.0f, 1.0f);
                    auto y = spPlatformModule->System()->VGetRandByFloat(-1.0f, 1.0f);

                    Core::Math::Vector3 p(static_cast<HE::Float32>(x), static_cast<HE::Float32>(y),
                                          0.0f);
                    aPos.Set(i, p);

                    Core::Math::Vector3 v(0.0f, 0.0f, 0.0f);
                    aVelocity.Set(i, v);

                    aColor.Set(i, Core::Math::RGB::White);
                }
            }
            // 各パーティクル位置と速度のデータ設定
            spDefaultRender->SetArrayPosParticleObject(particleObjHandle, aPos);
            spDefaultRender->SetArrtyVelocityParticelObject(particleObjHandle, aVelocity);
        }

        pWindow->VShow();
    }

    while (spPlatformModule->VIsQuit() == FALSE)
    {
        moduleManager.BeforeUpdate(0);
        moduleManager.Update(0);
        moduleManager.LateUpdate(0);
    }

    EXPECT_EQ(moduleManager.Release(), TRUE);
}
#endif

/// <summary>
/// SDL2File basic load tests
/// </summary>
TEST(HobbyPlugin_PlatformSDL2, FileLoad)
{
    File file;
    file.SetCurrentDir(HE_STR_TEXT("Resources/SDL2/File"));

    {
        auto [pData, size] = file.VLoadBinary(HE_STR_TEXT("Test.json"));
        EXPECT_NE(pData, nullptr);
        EXPECT_LT(0u, size);
        HE_SAFE_DELETE_MEM(pData);
    }

    {
        auto [pText, size] = file.VLoadText(HE_STR_TEXT("Test.xml"));
        EXPECT_NE(pText, nullptr);
        EXPECT_LT(0u, size);
        std::string str(pText, size);
        EXPECT_NE(str.find("<ui"), std::string::npos);
        HE_SAFE_DELETE_MEM(pText);
    }
}

/// <summary>
/// SDL2System random number tests
/// </summary>
TEST(HobbyPlugin_PlatformSDL2, SystemRand)
{
    System sys;
    EXPECT_EQ(sys.VSetSeedRand(12345), TRUE);
    auto a = sys.VGetRand(1000);
    EXPECT_EQ(sys.VSetSeedRand(12345), TRUE);
    auto b = sys.VGetRand(1000);
    EXPECT_EQ(a, b);

    auto f = sys.VGetRandByFloat(0.0f, 1.0f);
    EXPECT_LE(0.0f, f);
    EXPECT_GE(1.0f, f);
}

/// <summary>
/// SDL2Time functions
/// </summary>
TEST(HobbyPlugin_PlatformSDL2, TimeNowSleep)
{
    Time time;
    auto start = time.VNowMSec();
    time.VSleepMSec(10);
    auto end = time.VNowMSec();
    EXPECT_LE(10u, end - start);
}
}  // namespace PlatformSDL2
