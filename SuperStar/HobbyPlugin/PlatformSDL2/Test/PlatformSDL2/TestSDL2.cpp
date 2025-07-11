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

    //--------------------------------------------------------------------------------------
    // 描画テスト(矩形/円/三角形)
    //--------------------------------------------------------------------------------------
    TEST(HobbyPlugin_PlatformSDL2, QuadDraw)
    {
        Core::Memory::SharedPtr<PlatformSDL2::PlatformSDL2Module> spSDL2Module =
            HE_MAKE_CUSTOM_SHARED_PTR((PlatformSDL2::PlatformSDL2Module));

        Module::ModuleManager moduleManager;
        EXPECT_EQ(moduleManager.AddModule(spSDL2Module), TRUE);

        EXPECT_EQ(moduleManager.Start(), TRUE);

        auto spPlatformModule = moduleManager.Get<Platform::PlatformModule>();
        {
            auto bRet =
                spPlatformModule->Font()->VLoad(Platform::EFontSize_64,
                                                HE_STR_TEXT("Resources/SDL2/Font/TestFont.ttf"));
            EXPECT_EQ(bRet, TRUE);
        }

        auto inputHandle = spPlatformModule->Input()->VCreateObject();

        class TestEvent : public Platform::EventFrameInterface
        {
        public:
            TestEvent(HE::Uint32 in_w, HE::Uint32 in_h) : _w(in_w), _h(in_h) {}

            void VDraw(Platform::MapDrawable& in_mDrawble) override final
            {
                EXPECT_EQ(in_mDrawble.Contains(PlatformSDL2::Drawable2D::StaticRTTI()), TRUE);

                auto pDrawable = reinterpret_cast<PlatformSDL2::Drawable2D*>(
                    in_mDrawble.FindKey(PlatformSDL2::Drawable2D::StaticRTTI())->_data.get());

                pDrawable->VPreDraw();
                {
                    auto screenLeftTopPos =
                        Core::Math::Vector2(-static_cast<HE::Float32>(_w) * 0.5f,
                                            static_cast<HE::Float32>(_h) * 0.5f);

                    Core::Math::RC::Rect2D rect(screenLeftTopPos._fX, screenLeftTopPos._fY, 32.f,
                                                32.0f);
                    pDrawable->Draw2DQuad(rect, Core::Math::EAnchor_Left, Core::Math::RGB::Red);

                    rect.Set(screenLeftTopPos._fX + 33.f, screenLeftTopPos._fY, 32.0f, 32.0f);
                    pDrawable->Draw2DQuad(rect, Core::Math::EAnchor_Left, Core::Math::RGB::Yellow);

                    rect.Set(screenLeftTopPos._fX, screenLeftTopPos._fY - 33.0f, 32.0f, 32.0f);
                    pDrawable->Draw2DQuad(rect, Core::Math::EAnchor_Left, Core::Math::RGB::Yellow);

                    Core::Common::FixedString1024 s2(
                        HE_STR_TEXT("タイトルルルルルルルルルルルルルルるあ"));
                    rect.Set(screenLeftTopPos._fX, screenLeftTopPos._fY - (32.0f * 5.0f),
                             (32.0f * s2.Length()) + (2.0f * (s2.Length() - 1)), 32.0f);
                    pDrawable->Draw2DQuad(rect, Core::Math::EAnchor_Left, Core::Math::RGB::Red);

                    pDrawable->Draw2DText(Core::Math::Vector2(screenLeftTopPos._fX,
                                                              screenLeftTopPos._fY - 32.0f * 5.f),
                                          Core::Math::EAnchor_Left, s2.Str(), 32, 2.0f,
                                          Core::Math::RGB::Blue);

                    Core::Common::FixedString1024 s3(HE_STR_TEXT("MMM"));
                    rect.Set(screenLeftTopPos._fX, screenLeftTopPos._fY - (64.0f * 5.0f),
                             (16.0f * s3.Length()) + (s3.Length() - 1), 16.0f);
                    pDrawable->Draw2DQuad(rect, Core::Math::EAnchor_Left, Core::Math::RGB::Red);

                    pDrawable->Draw2DText(Core::Math::Vector2(screenLeftTopPos._fX,
                                                              screenLeftTopPos._fY - 64.0f * 5.f),
                                          Core::Math::EAnchor_Left, s3.Str(), 16, 1.0f,
                                          Core::Math::RGB::Blue);

                    rect.Set(.0f, .0f, 32.0f, 32.0f);
                    pDrawable->Draw2DQuad(rect, Core::Math::EAnchor_Center, Core::Math::RGB::Red);

                    rect.Set(.0f, .0f, 32.0f, 32.0f);
                    pDrawable->Draw2DQuad(rect, Core::Math::EAnchor_Left, Core::Math::RGB::White);

                    rect.Set(screenLeftTopPos._fX, -static_cast<HE::Float32>(_h) * 0.5f + 32.0f,
                             32.0f, 32.0f);
                    pDrawable->Draw2DQuad(rect, Core::Math::EAnchor_Left, Core::Math::RGB::White);

                    rect.Set(static_cast<HE::Float32>(_w) * 0.5f - 32.0f,
                             static_cast<HE::Float32>(_h) * 0.5f, 32.0f, 32.0f);
                    pDrawable->Draw2DQuad(rect, Core::Math::EAnchor_Left, Core::Math::RGB::White);

                    rect.Set(static_cast<HE::Float32>(_w) * 0.5f - 32.0f,
                             -static_cast<HE::Float32>(_h) * 0.5f + 32.0f, 32.0f, 32.0f);
                    pDrawable->Draw2DQuad(rect, Core::Math::EAnchor_Left, Core::Math::RGB::White);

                    Core::Math::Vector2 pos(.0f, .0f);
                    pDrawable->Draw2DCircle(pos, Core::Math::EAnchor_Left, 32,
                                            Core::Math::RGB::White);

                    pDrawable->Draw2DCircle(pos, Core::Math::EAnchor_Center, 32,
                                            Core::Math::RGB::White);

                    pos.Set(.0f, 0.f);
                    pDrawable->Draw2DTriangle(pos, Core::Math::EAnchor_Center, 0.0f, 32.0f,
                                              Core::Math::RGB::Blue);

                    pos.Set(200.f, 0.f);
                    pDrawable->Draw2DTriangle(pos, Core::Math::EAnchor_Left, -90.0f, 32.0f,
                                              Core::Math::RGB::White);
                }
                pDrawable->VPostDraw();
            }

        private:
            HE::Uint32 _w;
            HE::Uint32 _h;
        };

        Core::Common::Handle frameHandle;
        const auto uW = 640u;
        const auto uH = 480u;
        {
            auto windowConfig    = Platform::FrameConfig(uW, uH, 1, TRUE, inputHandle,
                                                         Platform::FrameConfig::EFlags_None);
            auto spGraphicModule = spPlatformModule->Graphic();
            frameHandle =
                spGraphicModule->VCreateFrame(windowConfig,
                                              HE_MAKE_CUSTOM_UNIQUE_PTR((TestEvent), uW, uH));
        }

        {
            auto spGraphicModule = spPlatformModule->Graphic();
            auto upDrawable2D    = HE_MAKE_CUSTOM_UNIQUE_PTR((PlatformSDL2::Drawable2D));
            upDrawable2D->VSetViewSize(uW, uH);
            upDrawable2D->SetFont(spPlatformModule->Font());
            spGraphicModule->VAddDrawable(std::move(upDrawable2D));
        }

        {
            auto spGraphicModule = spPlatformModule->Graphic();
            auto pFrame          = spGraphicModule->VGetFrame(frameHandle);
            pFrame->VShow();
        }

        while (spPlatformModule->VIsQuit() == FALSE)
        {
            moduleManager.BeforeUpdate(0);
            moduleManager.Update(0);
            moduleManager.LateUpdate(0);
        }

        EXPECT_EQ(moduleManager.Release(), TRUE);
    }

    //--------------------------------------------------------------------------------------
    // 描画テスト(パーティクル)
    //--------------------------------------------------------------------------------------
    TEST(HobbyPlugin_PlatformSDL2, ParticleDraw)
    {
        Core::Memory::SharedPtr<PlatformSDL2::PlatformSDL2Module> spSDL2Module =
            HE_MAKE_CUSTOM_SHARED_PTR((PlatformSDL2::PlatformSDL2Module));

        Module::ModuleManager moduleManager;
        EXPECT_EQ(moduleManager.AddModule(spSDL2Module), TRUE);

        EXPECT_EQ(moduleManager.Start(), TRUE);

        auto spPlatformModule = moduleManager.Get<Platform::PlatformModule>();
        {
            auto bRet =
                spPlatformModule->Font()->VLoad(Platform::EFontSize_64,
                                                HE_STR_TEXT("Resources/SDL2/Font/TestFont.ttf"));
            EXPECT_EQ(bRet, TRUE);
        }

        auto inputHandle = spPlatformModule->Input()->VCreateObject();

        class ParticleEvent : public Platform::EventFrameInterface
        {
        public:
            void SetHandle(const Core::Common::Handle in_handle) { _particleObj = in_handle; }

            void VDraw(Platform::MapDrawable& in_mDrawble) override final
            {
                EXPECT_EQ(in_mDrawble.Contains(PlatformSDL2::Drawable2D::StaticRTTI()), TRUE);

                auto pDrawable = reinterpret_cast<PlatformSDL2::Drawable2D*>(
                    in_mDrawble.FindKey(PlatformSDL2::Drawable2D::StaticRTTI())->_data.get());

                pDrawable->VPreDraw();
                {
                    pDrawable->Draw2DPartical(_particleObj, _pos);
                    _pos._fY += 0.5f;
                }
                pDrawable->VPostDraw();
            }

        private:
            Core::Common::Handle _particleObj;
            Core::Math::Vector3 _pos{.0f, .0f, 0.0f};
        };

        Core::Common::Handle frameHandle;
        const auto uW = 640u;
        const auto uH = 480u;

        auto upEvent = HE_MAKE_CUSTOM_UNIQUE_PTR((ParticleEvent));
        auto* pEvent = upEvent.get();

        {
            auto windowConfig    = Platform::FrameConfig(uW, uH, 1, TRUE, inputHandle,
                                                         Platform::FrameConfig::EFlags_None);
            auto spGraphicModule = spPlatformModule->Graphic();
            frameHandle          = spGraphicModule->VCreateFrame(windowConfig, std::move(upEvent));
        }

        PlatformSDL2::Drawable2D* pDrawable2D = nullptr;
        {
            auto spGraphicModule = spPlatformModule->Graphic();
            auto upDrawable2D    = HE_MAKE_CUSTOM_UNIQUE_PTR((PlatformSDL2::Drawable2D));
            pDrawable2D          = upDrawable2D.get();
            upDrawable2D->VSetViewSize(uW, uH);
            upDrawable2D->SetFont(spPlatformModule->Font());
            spGraphicModule->VAddDrawable(std::move(upDrawable2D));
        }

        Core::Common::Handle particleObjHandle = pDrawable2D->CreateParticleObject(1000);

        Core::Common::FixedArray<Core::Math::Vector3, 1000> aPos;
        Core::Common::FixedArray<Core::Math::Vector3, 1000> aVelocity;
        for (HE::Uint32 i = 0; i < 1000; ++i)
        {
            auto x = spPlatformModule->System()->VGetRandByFloat(-1.0f, 1.0f);
            auto y = spPlatformModule->System()->VGetRandByFloat(-1.0f, 1.0f);

            Core::Math::Vector3 p(static_cast<HE::Float32>(x), static_cast<HE::Float32>(y), 0.0f);
            aPos.Set(i, p);

            Core::Math::Vector3 v(0.0f, 0.0f, 0.0f);
            aVelocity.Set(i, v);
        }

        pDrawable2D->SetArrayPosParticleObject(particleObjHandle, aPos);
        pDrawable2D->SetArrtyVelocityParticelObject(particleObjHandle, aVelocity);

        pEvent->SetHandle(particleObjHandle);

        {
            auto spGraphicModule = spPlatformModule->Graphic();
            auto pFrame          = spGraphicModule->VGetFrame(frameHandle);
            pFrame->VShow();
        }

        while (spPlatformModule->VIsQuit() == FALSE)
        {
            moduleManager.BeforeUpdate(0);
            moduleManager.Update(0);
            moduleManager.LateUpdate(0);
        }

        EXPECT_EQ(moduleManager.Release(), TRUE);
    }

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
