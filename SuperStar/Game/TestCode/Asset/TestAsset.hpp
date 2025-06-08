#pragma once

// ゲームプロジェクトはアプリプロジェクトでコードの実態を入れ込むためにcppファイルもインクルードしないといけない
#include "../PlatformSDL2/PlatformSDL2Module.h"
#include "Code/Asset/ParamaterAssetData.cpp"
#include "Code/Asset/ParamaterAssetData.h"
#include "Engine/Common/Function.h"
#include "Engine/Engine.h"

TEST_CASE("Test Paramater Asset Load")
{
    StartupEngineByUnitTest<AssetManager::AssetManagerModule, PlatformSDL2::PlatformSDL2Module>(
        []()
        {
            auto pAssetManagerModule =
                HE_ENGINE.ModuleManager().Get<AssetManager::AssetManagerModule>();
            pAssetManagerModule->SetCurrentDir(HE_STR_TEXT("Assets"));
            auto handle = pAssetManagerModule->Load<Game::Asset::ParamaterAssetData>(
                HE_STR_TEXT("PlayerParamater"), Core::File::Path("Paramater/Test.json"));

            CHECK(handle.Null() == FALSE);
            Game::Asset::ParamaterAssetData& rAsset =
                pAssetManagerModule->GetAsset<Game::Asset::ParamaterAssetData>(handle);
            CHECK(rAsset.Version() == 1);
            HE_LOG_LINE(HE_STR_TEXT("バージョン: %d"), rAsset.Version());

            auto uHp = rAsset.GetUInt32ByName("player_default", "hp");
            CHECK(uHp == 3);
            HE_LOG_LINE(HE_STR_TEXT("HP: %d"), uHp);

            auto fMoveSpeed = rAsset.GetFloat32ByName("player_default", "move_speed");
            CHECK(fMoveSpeed == 7.0f);
            HE_LOG_LINE(HE_STR_TEXT("MoveSpeed: %f"), fMoveSpeed);

            pAssetManagerModule->Unload(handle);
            return TRUE;
        });
}
