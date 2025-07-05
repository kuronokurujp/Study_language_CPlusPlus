cmake_minimum_required(VERSION 3.26)

# プロジェクトの共通処理
include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/Project.cmake NO_POLICY_SCOPE)
# エンジン設定
include(${CMAKE_SOURCE_DIR}/HobbyEngine/Config.cmake NO_POLICY_SCOPE)
# プラグインビルドの共通処理
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICY_SCOPE)

message("build localization plugin lib!")

# 静的ライブラリの設定
project_configure_static_lib_target(${LOCALIZATION_PLUGIN_NAME})

# プロジェクトの設定
project_configure_target(${LOCALIZATION_PLUGIN_NAME})

# ターゲットへファイル群の設定
project_configure_files_target(${LOCALIZATION_PLUGIN_NAME}
    ${LOCALIZATION_PLUGIN_TARGET_COMMON_FILES}
    ${LOCALIZATION_PLUGIN_TARGET_SRC_FILES}
    ${LOCALIZATION_PLUGIN_TARGET_INC_FILES}
)

# エンジンのインクルードパス
target_include_directories(${LOCALIZATION_PLUGIN_NAME} PUBLIC ${ENGINE_INC_DIR})

# プロジェクトが参照できるインクルードディレクトリのパス
target_include_directories(${LOCALIZATION_PLUGIN_NAME} PUBLIC ${LOCALIZATION_PLUGIN_INC_DIR})

# 依存プラグインのインクルードディレクトリ
target_include_directories(${LOCALIZATION_PLUGIN_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../AssetManager/)

# 依存ライブラリのリンク
target_link_libraries(${LOCALIZATION_PLUGIN_NAME}
    PUBLIC
        ${ENGINE_NAME}
)

# テスト用サブディレクトリを追加
add_subdirectory(Test)
