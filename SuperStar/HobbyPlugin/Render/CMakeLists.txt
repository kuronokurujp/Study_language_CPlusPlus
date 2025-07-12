cmake_minimum_required(VERSION 3.26)

# Engine common cmake
include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/Project.cmake NO_POLICY_SCOPE)
# VCPkgの設定
include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/VCPkg.cmake NO_POLICY_SCOPE)
# エンジン設定
include(${CMAKE_SOURCE_DIR}/HobbyEngine/Config.cmake NO_POLICY_SCOPE)
# Plugin common info
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICY_SCOPE)

message("build render plugin lib!")

# Static library target
project_configure_static_lib_target(${PLUGIN_RENDER_NAME})

# Project configuration
project_configure_target(${PLUGIN_RENDER_NAME})

# Files for the target
project_configure_files_target(${PLUGIN_RENDER_NAME}
    ${PLUGIN_RENDER_TARGET_COMMON_FILES}
    ${PLUGIN_RENDER_TARGET_SRC_FILES}
    ${PLUGIN_RENDER_TARGET_INC_FILES}
)

target_link_libraries(${PLUGIN_RENDER_NAME}
    PUBLIC
        # エンジンのライブラリをリンク
        ${ENGINE_NAME}
)

# Plugin include path
target_include_directories(${PLUGIN_RENDER_NAME} PUBLIC ${PLUGIN_RENDER_INC_DIR})

add_subdirectory(Test)
