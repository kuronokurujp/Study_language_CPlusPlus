// 以下のマクロ定義するとCatch2側でエントリーポイント(main)関数が定義される
#define CATCH_CONFIG_RUNNER
#define HE_UNIT_TEST_MODE

#include "Engine/MiniEngine.h"
#include "TestCommon.h"
#include "ThirdParty/Catch.hpp"

// デバッグビルドのみ有効
#ifdef HE_ENGINE_DEBUG

// 外部モジュール一覧
#include <fcntl.h>
#include <io.h>

// Catch.hppのカスタムメイン関数を実装
#ifndef __OBJC__

#ifndef CATCH_INTERNAL_CDECL
#ifdef _MSC_VER
#define CATCH_INTERNAL_CDECL __cdecl
#else
#define CATCH_INTERNAL_CDECL
#endif
#endif

#if defined(CATCH_CONFIG_WCHAR) && defined(CATCH_PLATFORM_WINDOWS) && defined(_UNICODE) && \
    !defined(DO_NOT_USE_WMAIN)
// Standard C/C++ Win32 Unicode wmain entry point
int CATCH_INTERNAL_CDECL wmain(int argc, wchar_t* argv[], wchar_t*[])
{
#ifdef HE_WIN
#ifdef HE_CHARACTER_CODE_UTF8
    // UTF-8 を使うようにコードページを設定
    // ウィンドウズだが文字コードはUTF8を使うのでwchar_tが使えないのでコンソール設定をUTF8にして日本語表示を文字化けしないようにする
    SetConsoleOutputCP(CP_UTF8);
#endif
#endif

#else
// Standard C/C++ main entry point
int CATCH_INTERNAL_CDECL main(int argc, char* argv[])
{
#endif
    const int ret = Catch::Session().run(argc, argv);

#ifdef HE_WIN
    ::TerminateProcess(::GetCurrentProcess(), 0);
#endif

    return ret;
}

#else  // __OBJC__

// Objective-C entry point
int main(int argc, char* const argv[])
{
#if !CATCH_ARC_ENABLED
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
#endif

    Catch::registerTestMethods();
    int result = Catch::Session().run(argc, (char**)argv);

#if !CATCH_ARC_ENABLED
    [pool drain];
#endif

    return result;
}

#endif  // __OBJC__

// TODO: 現在はコメントアウトでどのテストON/OFFにしている
// TODO: テスト追加や削除などはコード上で行うのではなく設定上でするとかしたい
#if 1
// エンジンのテスト一覧

// マクロテストコード
#include "EngineTest/Common/TestMacro.hpp"

// UUIDのテストコード
#include "EngineTest/Common/TestUUID.hpp"

// ハッシュのテストコード
#include "EngineTest/Common/TestHash.hpp"
// カスタム配列のテストコード
#include "EngineTest/Common/TestArray.hpp"
// カスタムVectorのテストコード
#include "EngineTest/Common/TestVector.hpp"
// カスタムStackのテストコード
#include "EngineTest/Common/TestStack.hpp"
// 文カスタム字列のテストコード
#include "EngineTest/Common/TestString.hpp"
// カスタムリストのテストコード
#include "EngineTest/Common/TestList.hpp"
// プール管理のテストコード
#include "EngineTest/Common/TestPoolManager.hpp"
// ランダム処理のテストコード
#include "EngineTest/Math/TestRandom.hpp"

// タスクのテストコード
#include "EngineTest/Task/TestTask.hpp"

// Mapテストコード
#include "EngineTest/Common/TestMap.hpp"

// メモリ処理のテストコード
#include "EngineTest/Memory/TestMemory.hpp"

// 非同期タスク処理のテストコード
#include "EngineTest/AsyncTask/TestAsyncTask.hpp"
// エンジン本体のテストコード
#include "EngineTest/TestEngine.hpp"

// エンジンのモジュール用のテスト一覧

// Luaモジュールテスト
#include "LuaTest/LuaModuleTest.hpp"

// AssetManagerモジュールテスト
#include "AssetManagerTest/AssetManagerModuleTest.hpp"

// ローカライズモジュールテスト
#include "LocalizationTest/LocalizationModuleTest.hpp"

// イベントテスト
#include "TestCode/Event/TestEvent.hpp"

#endif
// GUIテスト
// SDL2のテスト
// #include "PlatformSDL2Test/PlatformSDL2ModuleTest.hpp"
// #include "GameDevGUITest/GameDevGUIModuleTest.hpp"

// #include "UITest/UIModuleTest.hpp"

#if 1
// ゲーム用のテスト一覧

// Lambdaテストコード
#include "EngineTest/Common/TestLambda.hpp"

// ゲーム用アセットのテスト
// ゲームプロジェクトはアプリプロジェクトでコードの実態を入れ込むためにcppファイルもインクルードしないといけない
#include "TestCode/Asset/TestAsset.hpp"
#endif

#endif
