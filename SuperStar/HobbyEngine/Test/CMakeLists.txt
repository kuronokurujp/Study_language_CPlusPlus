cmake_minimum_required(VERSION 3.26)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/VCPkg.cmake NO_POLICY_SCOPE)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Project.cmake NO_POLICY_SCOPE)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Test.cmake NO_POLICY_SCOPE)
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICY_SCOPE)

# テスト用実行ファイルを定義
add_executable(${ENGINE_TEST_NAME}
	${ENGINE_TARGET_TEST_SRC_FILES}
)

# プロジェクト設定
project_configure_target(${ENGINE_TEST_NAME})

# ターゲットへファイル群の設定
project_configure_files_target(${ENGINE_TEST_NAME}
	${ENGINE_TARGET_TEST_SRC_FILES}
)

# .vcpkg.jsonファイルからvcpkgツールを使ってライブラリをインストール
vcpkg_install_package(${CMAKE_CURRENT_SOURCE_DIR} "x64-windows-static")

# gtest を利用したテストの環境設定
# ここでプロジェクト全体で利用するgtestの設定を行う
# プラグインではエンジンの設定を必ず行うのでgtestの設定済み状態なのでプラグインではgtestの取得は不要
test_gtest_env(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/VcPkg/x64-windows-static)

# テスト専用のプロジェクト設定
test_configure_target(${ENGINE_TEST_NAME} ${ENGINE_NAME})
