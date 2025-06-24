cmake_minimum_required(VERSION 3.26)

# プロジェクトの共通処理
include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/Project.cmake NO_POLICY_SCOPE)
# エンジン設定
include(${CMAKE_SOURCE_DIR}/HobbyEngine/Config.cmake NO_POLICY_SCOPE)
# プラグインビルドの共通処理
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICY_SCOPE)

message("build asset manager plugin lib!")

# 静的ライブラリの設定
project_configure_static_lib_target(${PLUGIN_ASSET_MANAGER_NAME})

# プロジェクトの設定
project_configure_target(${PLUGIN_ASSET_MANAGER_NAME})

# ターゲットへファイル群の設定
project_configure_files_target(${PLUGIN_ASSET_MANAGER_NAME}
    ${PLUGIN_ASSET_MANAGER_TARGET_COMMON_FILES}
    ${PLUGIN_ASSET_MANAGER_TARGET_SRC_FILES}
    ${PLUGIN_ASSET_MANAGER_TARGET_INC_FILES}
)

# エンジンのインクルードパス
target_include_directories(${PLUGIN_ASSET_MANAGER_NAME} PUBLIC ${ENGINE_INC_DIR})

# プロジェクトが参照できるインクルードディレクトリのパス
target_include_directories(${PLUGIN_ASSET_MANAGER_NAME} PUBLIC ${PLUGIN_ASSET_MANAGER_INC_DIR})
# プロジェクトのみが参照できるインクルードディレクトリのパス
target_include_directories(${PLUGIN_ASSET_MANAGER_NAME} PRIVATE ${PLUGIN_ASSET_MANAGER_THIRD_PARTY_DIR})

# テスト用サブディレクトリを追加
add_subdirectory(Test)
