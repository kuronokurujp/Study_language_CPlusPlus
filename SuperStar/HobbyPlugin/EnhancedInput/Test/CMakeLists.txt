cmake_minimum_required(VERSION 3.26)

# Engineが用意したcmake共通をインクルード
include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/VCPkg.cmake NO_POLICY_SCOPE)
include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/Project.cmake NO_POLICY_SCOPE)
include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/Test.cmake NO_POLICY_SCOPE)
# Actorモジュール設定
include(${CMAKE_SOURCE_DIR}/HobbyPlugin/Actor/Config.cmake NO_POLICY_SCOPE)
# Eventモジュール設定
include(${CMAKE_SOURCE_DIR}/HobbyPlugin/Event/Config.cmake NO_POLICY_SCOPE)

include(${CMAKE_CURRENT_SOURCE_DIR}/../Config.cmake NO_POLICY_SCOPE)
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICY_SCOPE)

# テスト用実行ファイルを定義
add_executable(${PLUGIN_ENHANCED_INPUT_TEST_NAME}
	${PLUGIN_ENHANCED_INPUT_TARGET_TEST_SRC_FILES}
)

# プロジェクト設定
project_configure_target(${PLUGIN_ENHANCED_INPUT_TEST_NAME})

# ターゲットへファイル群の設定
project_configure_files_target(${PLUGIN_ENHANCED_INPUT_TEST_NAME}
	${PLUGIN_ENHANCED_INPUT_TARGET_TEST_SRC_FILES}
)

# テスト専用のプロジェクト設定
test_configure_target(
	${PLUGIN_ENHANCED_INPUT_TEST_NAME}
	${PLUGIN_ENHANCED_INPUT_NAME}
)
