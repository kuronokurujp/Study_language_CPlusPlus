#pragma once

// ゲームプロジェクトはアプリプロジェクトでコードの実態を入れ込むためにcppファイルもインクルードしないといけない
#include "Code/Asset/ParamaterAssetData.cpp"
#include "Code/Asset/ParamaterAssetData.h"

#define HE_UNIT_TEST_MODE
#include "../DxLib/DXLibModule.h"
#include "Engine/Common/Function.h"
#include "Engine/Engine.h"

TEST_CASE("Test Paramater Asset Load")
{
    UnitTestRunnerByModuleOnly<AssetManager::AssetManagerModule, DXLib::DXLibModule>(
        []()
        {
            auto pAssetManagerModule =
                HE_ENGINE.ModuleManager().Get<AssetManager::AssetManagerModule>();
            pAssetManagerModule->SetMountDir(HE_STR_TEXT("Assets"));
            auto handle = pAssetManagerModule->Load<Game::Asset::ParamaterAssetData>(
                HE_STR_TEXT("PlayerParamater"), Core::File::Path("Paramater/Player.json"));

            CHECK(handle.Null() == FALSE);
            Game::Asset::ParamaterAssetData& rAsset =
                pAssetManagerModule->GetAsset<Game::Asset::ParamaterAssetData>(handle);
            CHECK(rAsset.Version() == 1);
            HE_LOG_LINE(HE_STR_TEXT("バージョン: %d"), rAsset.Version());

            pAssetManagerModule->Unload(handle);
            return TRUE;
        });
}
