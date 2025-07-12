cmake_minimum_required(VERSION 3.26)

# Engineが用意したcmake共通をインクルード
include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/VCPkg.cmake NO_POLICY_SCOPE)
include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/Project.cmake NO_POLICY_SCOPE)
include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/Test.cmake NO_POLICY_SCOPE)
# TODO: 将来DX12など別プラットフォームを利用するケースがあるので分岐する必要がある
include(${CMAKE_SOURCE_DIR}/HobbyPlugin/PlatformSDL2/Config.cmake NO_POLICY_SCOPE)

include(${CMAKE_CURRENT_SOURCE_DIR}/../Config.cmake NO_POLICY_SCOPE)
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICY_SCOPE)

# テスト用実行ファイルを定義
add_executable(${PLUGIN_ASSET_MANAGER_TEST_NAME}
	${PLUGIN_ASSET_MANAGER_TARGET_TEST_SRC_FILES}
)

# プロジェクト設定
project_configure_target(${PLUGIN_ASSET_MANAGER_TEST_NAME})

# ターゲットへファイル群の設定
project_configure_files_target(${PLUGIN_ASSET_MANAGER_TEST_NAME}
	${PLUGIN_ASSET_MANAGER_TARGET_TEST_SRC_FILES}
)

# テスト専用のプロジェクト設定
test_configure_target(
	${PLUGIN_ASSET_MANAGER_TEST_NAME}
	${ENGINE_NAME}
	${PLUGIN_ASSET_MANAGER_NAME}
	${PLUGIN_PLATFORMSDL2_NAME}
)

# POST_BUILD で Resources フォルダをコピー
add_custom_command(
    TARGET ${PLUGIN_ASSET_MANAGER_TEST_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
            "$<TARGET_FILE_DIR:${PLUGIN_ASSET_MANAGER_TEST_NAME}>/Resources"
    COMMENT "Copying Resources to output dir"
)
