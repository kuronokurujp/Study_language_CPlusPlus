cmake_minimum_required(VERSION 3.26)

# プロジェクトの共通処理
include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/Project.cmake NO_POLICY_SCOPE)
# エンジン設定
include(${CMAKE_SOURCE_DIR}/HobbyEngine/Config.cmake NO_POLICY_SCOPE)
# Actorモジュール設定
include(${CMAKE_SOURCE_DIR}/HobbyPlugin/Actor/Config.cmake NO_POLICY_SCOPE)
# Eventモジュール設定
include(${CMAKE_SOURCE_DIR}/HobbyPlugin/Event/Config.cmake NO_POLICY_SCOPE)
# EnhancedInputモジュール設定
include(${CMAKE_SOURCE_DIR}/HobbyPlugin/EnhancedInput/Config.cmake NO_POLICY_SCOPE)
# プラグインビルドの共通処理
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICY_SCOPE)

message("build level plugin lib!")

# 静的ライブラリの設定
project_configure_static_lib_target(${PLUGIN_LEVEL_NAME})

# プロジェクトの設定
project_configure_target(${PLUGIN_LEVEL_NAME})

# ターゲットへファイル群の設定
project_configure_files_target(${PLUGIN_LEVEL_NAME}
    ${PLUGIN_LEVEL_TARGET_COMMON_FILES}
    ${PLUGIN_LEVEL_TARGET_SRC_FILES}
    ${PLUGIN_LEVEL_TARGET_INC_FILES}
)

# プロジェクトが参照できるインクルードディレクトリのパス
target_include_directories(${PLUGIN_LEVEL_NAME} PUBLIC ${PLUGIN_LEVEL_INC_DIR})

# 利用するモジュールのインクルードパス
target_include_directories(${PLUGIN_LEVEL_NAME}
    PUBLIC
        ${ENGINE_INC_DIR}
        ${PLUGIN_EVENT_INC_DIR}
        ${PLUGIN_ACTOR_INC_DIR}
        ${PLUGIN_ENHANCED_INPUT_INC_DIR}
)

# 依存ライブラリのリンク
target_include_directories(${PLUGIN_LEVEL_NAME}
    PUBLIC
        ${ENGINE_NAME}
        ${PLUGIN_EVENT_NAME}
        ${PLUGIN_ACTOR_NAME}
        ${PLUGIN_ENHANCED_INPUT_NAME}
)

# サブディレクトリ追加
add_subdirectory(Test)
