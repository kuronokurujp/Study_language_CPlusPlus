cmake_minimum_required(VERSION 3.26)

# エンジン提供の共通情報
include(${CMAKE_CURRENT_SOURCE_DIR}/HobbyEngine/cmake/Common.cmake NO_POLICY_SCOPE)
# エンジン提供のProject構築情報
include(${CMAKE_CURRENT_SOURCE_DIR}/HobbyEngine/cmake/Project.cmake NO_POLICY_SCOPE)
# エンジン提供のエンコーディングをUTF8構築
include(${CMAKE_CURRENT_SOURCE_DIR}/HobbyEngine/cmake/UTF8.cmake NO_POLICY_SCOPE)

# ゲームアプリ
# include(${CMAKE_CURRENT_SOURCE_DIR}/Game/Config.cmake NO_POLICY_SCOPE)

# ゲームエンジン
include(${CMAKE_CURRENT_SOURCE_DIR}/HobbyEngine/Config.cmake NO_POLICY_SCOPE)
# SDL2プラットフォーム
include(${CMAKE_CURRENT_SOURCE_DIR}/HobbyPlugin/PlatformSDL2/Config.cmake NO_POLICY_SCOPE)
# 描画プラグイン
include(${CMAKE_CURRENT_SOURCE_DIR}/HobbyPlugin/Render/Config.cmake NO_POLICY_SCOPE)
# アクタープラグイン
include(${CMAKE_CURRENT_SOURCE_DIR}/HobbyPlugin/Actor/Config.cmake NO_POLICY_SCOPE)
# アセット管理プラグイン
include(${CMAKE_CURRENT_SOURCE_DIR}/HobbyPlugin/AssetManager/Config.cmake NO_POLICY_SCOPE)
# レベルプラグイン
include(${CMAKE_CURRENT_SOURCE_DIR}/HobbyPlugin/Level/Config.cmake NO_POLICY_SCOPE)
# ローカライズプラグイン
include(${CMAKE_CURRENT_SOURCE_DIR}/HobbyPlugin/Localization/Config.cmake NO_POLICY_SCOPE)
# UIプラグイン
# include(${CMAKE_CURRENT_SOURCE_DIR}/HobbyPlugin/UI/Config.cmake NO_POLICY_SCOPE)
# EnhancedInputプラグイン
include(${CMAKE_CURRENT_SOURCE_DIR}/HobbyPlugin/EnhancedInput/Config.cmake NO_POLICY_SCOPE)
# Eventプラグイン
include(${CMAKE_CURRENT_SOURCE_DIR}/HobbyPlugin/Event/Config.cmake NO_POLICY_SCOPE)
# Luaプラグイン
include(${CMAKE_CURRENT_SOURCE_DIR}/HobbyPlugin/Lua/Config.cmake NO_POLICY_SCOPE)
# TODO: 以下はデバッグのみ利用(リリース版では含めないようにする)
# ランタイムのDebugプラグイン
# include(${CMAKE_CURRENT_SOURCE_DIR}/HobbyPlugin/RuntimeDebug/Config.cmake NO_POLICY_SCOPE)
# ゲーム開発用のGUIプラグイン
# include(${CMAKE_CURRENT_SOURCE_DIR}/HobbyPlugin/GameDevGUI/Config.cmake NO_POLICY_SCOPE)

# VSソリューション名をワークスペース名にしている
set(WORKSPACE_NAME SuperStar)

# ワークスペース名と使用言語C++をプロジェクトに設定.
project(${WORKSPACE_NAME} VERSION 2.0 LANGUAGES C CXX)

# 処理開始のBootを取り込む
include(${CMAKE_CURRENT_SOURCE_DIR}/HobbyEngine/cmake/Boot.cmake NO_POLICY_SCOPE)

option(
  CORE_CRL_MD
  "Use Cubism Core that is multithread-specific and DLL-specific version"
  OFF
)

# プロジェクトで参照するファイル群
# vcpkgから取得するパッケージのライブラリタイプ
set(CACHE_PROJECT_VCPKG_STATIC_LIB_TYPE "x64-windows-static" CACHE STRING "" FORCE)
# vcpkgのルートディレクトリ
set(CHACHE_VCPKG_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/vcpkg" CACHE STRING "Vcpkg toolchain file" FORCE)

# 環境変数からvcpkgのパスを取得してツールチェインを設定
set(CMAKE_TOOLCHAIN_FILE "$CACHE{CHACHE_VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file" FORCE)

# ビルド成果物を出力するディレクトリ指定.
# 実行ファイルの出力先
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin/${WORKSPACE_NAME})
# 静的ライブラリファイルの出力先
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin/${WORKSPACE_NAME})
# 共有ライブラリの出力先
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin/${WORKSPACE_NAME})

# Set configuration (Release and Debug only).
# set(CMAKE_CONFIGURATION_TYPES Debug Release UnitTest
#   CACHE STRING "Configurations" FORCE
# )
set(CMAKE_CONFIGURATION_TYPES Debug Release
  CACHE STRING "Configurations" FORCE
)

# 各プロジェクトを設定
# TODO: ファイルを構成を大幅に変更で一つ一つ対応
add_subdirectory(HobbyEngine)
add_subdirectory(HobbyPlugin/PlatformSDL2)
add_subdirectory(HobbyPlugin/Render)
add_subdirectory(HobbyPlugin/Actor)
add_subdirectory(HobbyPlugin/AssetManager)
add_subdirectory(HobbyPlugin/Level)
add_subdirectory(HobbyPlugin/Localization)
# add_subdirectory(HobbyPlugin/UI)
add_subdirectory(HobbyPlugin/EnhancedInput)
add_subdirectory(HobbyPlugin/Event)
add_subdirectory(HobbyPlugin/Lua)
# TODO: 以下のはデバッグのみ利用(リリース版では含めないようにする)
# add_subdirectory(HobbyPlugin/RuntimeDebug)
# add_subdirectory(HobbyPlugin/GameDevGUI)


# add_subdirectory(Game)

# ライブラリ名をリストにまとめる
set(PLUGIN_LIBRARIES
# TODO: デバッグのみ入れる
#   ${GAMEDEVGUI_PLUGIN_NAME}
# TODO: デバッグのみ入れる
#   ${RUNTIMEDEBUG_PLUGIN_NAME}
  ${LUA_PLUGIN_NAME}
  ${EVENT_PLUGIN_NAME}
  ${ENHANCED_INPUT_PLUGIN_NAME}
#   ${UI_PLUGIN_NAME}
  ${LOCALIZATION_PLUGIN_NAME}
  ${PLUGIN_LEVEL_NAME}
  ${ASSET_MANAGER_PLUGIN_NAME}
  ${RENDER_PLUGIN_NAME}
  ${PLUGIN_ACTOR_NAME}
  ${PLUGIN_PLATFORMSDL2_NAME}
  ${ENGINE_NAME}
)

# ゲームアプリがリンクするライブラリ
# 設定プロジェクトのライブラリ・インクルードディレクトリ登録
# target_link_libraries(${APP_NAME} ${PLUGIN_LIBRARIES})
