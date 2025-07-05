cmake_minimum_required(VERSION 3.26)

# プロジェクトの共通処理
include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/Project.cmake NO_POLICY_SCOPE)
# エンジン設定
include(${CMAKE_SOURCE_DIR}/HobbyEngine/Config.cmake NO_POLICY_SCOPE)
# プラグインビルドの共通処理
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICY_SCOPE)
# 依存しているモジュール一覧
include(${CMAKE_SOURCE_DIR}/HobbyPlugin/AssetManager/Config.cmake NO_POLICY_SCOPE)

message("build localization plugin lib!")

# 静的ライブラリの設定
project_configure_static_lib_target(${PLUGIN_LOCALIZATION_NAME})

# プロジェクトの設定
project_configure_target(${PLUGIN_LOCALIZATION_NAME})

# ターゲットへファイル群の設定
project_configure_files_target(${PLUGIN_LOCALIZATION_NAME}
    ${PLUGIN_LOCALIZATION_TARGET_COMMON_FILES}
    ${PLUGIN_LOCALIZATION_TARGET_SRC_FILES}
    ${PLUGIN_LOCALIZATION_TARGET_INC_FILES}
)

# エンジンのインクルードパス
target_include_directories(${PLUGIN_LOCALIZATION_NAME} PUBLIC ${ENGINE_INC_DIR})

# プロジェクトが参照できるインクルードディレクトリのパス
target_include_directories(${PLUGIN_LOCALIZATION_NAME} PUBLIC ${PLUGIN_LOCALIZATION_INC_DIR})

# 依存プラグインのインクルードディレクトリ
target_include_directories(${PLUGIN_LOCALIZATION_NAME} PRIVATE ${PLUGIN_ASSET_MANAGER_INC_DIR})

# 依存ライブラリのリンク
target_link_libraries(${PLUGIN_LOCALIZATION_NAME}
    PUBLIC
        ${ENGINE_NAME}
        ${PLUGIN_ASSET_MANAGER_NAME}
)

# テスト用サブディレクトリを追加
add_subdirectory(Test)
