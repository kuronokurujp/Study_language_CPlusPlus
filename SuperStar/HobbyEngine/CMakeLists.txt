cmake_minimum_required(VERSION 3.26)

# 共通情報を取得.
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICY_SCOPE)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Project.cmake NO_POLICY_SCOPE)

message("build engine lib!")

# 静的ライブラリの設定
project_configure_static_lib_target(${ENGINE_NAME})

# プロジェクトの設定
project_configure_target(${ENGINE_NAME})

# ターゲットへファイル群の設定
project_configure_files_target(${ENGINE_NAME}
    ${ENGINE_TARGET_COMMON_FILES}
    ${ENGINE_TARGET_SRC_FILES}
    ${ENGINE_TARGET_INC_FILES}
)

# Windows特有のライブラリをリンク
if (MSVC)
    target_link_libraries(${ENGINE_NAME} PRIVATE PathCch)
endif()

# プロジェクトが参照できるインクルードディレクトリのパス.
target_include_directories(${ENGINE_NAME} PUBLIC ${ENGINE_INC_DIR})

# テスト用サブディレクトリを追加
add_subdirectory(Test)
