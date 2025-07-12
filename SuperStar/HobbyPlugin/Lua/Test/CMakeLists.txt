cmake_minimum_required(VERSION 3.26)

# Engineが用意したcmake共通をインクルード
include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/VCPkg.cmake NO_POLICY_SCOPE)
include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/Project.cmake NO_POLICY_SCOPE)
include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/Test.cmake NO_POLICY_SCOPE)

include(${CMAKE_CURRENT_SOURCE_DIR}/../Config.cmake NO_POLICY_SCOPE)
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICY_SCOPE)

# テスト用実行ファイルを定義
add_executable(${PLUGIN_LUA_TEST_NAME}
        ${PLUGIN_LUA_TARGET_TEST_SRC_FILES}
)

# プロジェクト設定
project_configure_target(${PLUGIN_LUA_TEST_NAME})

# ターゲットへファイル群の設定
project_configure_files_target(${PLUGIN_LUA_TEST_NAME}
        ${PLUGIN_LUA_TARGET_TEST_SRC_FILES}
)

# テスト専用のプロジェクト設定
test_configure_target(
        ${PLUGIN_LUA_TEST_NAME}
        ${PLUGIN_LUA_NAME}
)
