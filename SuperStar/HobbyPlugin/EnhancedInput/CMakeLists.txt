# プラグインビルドの共通処理.
include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/Project.cmake NO_POLICY_SCOPE)
# エンジン設定
include(${CMAKE_SOURCE_DIR}/HobbyEngine/Config.cmake NO_POLICY_SCOPE)
# Actorモジュール設定
include(${CMAKE_SOURCE_DIR}/HobbyPlugin/Actor/Config.cmake NO_POLICY_SCOPE)
# Eventモジュール設定
include(${CMAKE_SOURCE_DIR}/HobbyPlugin/Event/Config.cmake NO_POLICY_SCOPE)

message("build enchancde input plugin lib!")

# 静的ライブラリの設定
project_configure_static_lib_target(${PLUGIN_ENHANCED_INPUT_NAME})

# プロジェクトの設定
project_configure_target(${PLUGIN_ENHANCED_INPUT_NAME})

# ターゲットへファイル群の設定
project_configure_files_target(${PLUGIN_ENHANCED_INPUT_NAME}
    ${PLUGIN_ENHANCED_INPUT_TARGET_COMMON_FILES}
    ${PLUGIN_ENHANCED_INPUT_TARGET_SRC_FILES}
    ${PLUGIN_ENHANCED_INPUT_TARGET_INC_FILES}
)

# 利用するモジュールのインクルードパス
target_include_directories(${PLUGIN_ENHANCED_INPUT_NAME}
    PUBLIC
        ${ENGINE_INC_DIR}
        ${PLUGIN_EVENT_INC_DIR}
        ${PLUGIN_ACTOR_INC_DIR}
)

# プロジェクトが参照できるインクルードディレクトリのパス
target_include_directories(${PLUGIN_ENHANCED_INPUT_NAME}
    PUBLIC
        ${PLUGIN_ENHANCED_INPUT_INC_DIR}
)

# テスト用サブディレクトリを追加
add_subdirectory(Test)
