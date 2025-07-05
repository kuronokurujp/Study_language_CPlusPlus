cmake_minimum_required(VERSION 3.26)

# Engine common cmake
include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/Project.cmake NO_POLICY_SCOPE)
# Engine configuration
include(${CMAKE_SOURCE_DIR}/HobbyEngine/Config.cmake NO_POLICY_SCOPE)
# Plugin configuration
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICY_SCOPE)

# Static library target
project_configure_static_lib_target(${PLUGIN_LUA_NAME})

# Project configuration
project_configure_target(${PLUGIN_LUA_NAME})

# Files for the target
project_configure_files_target(${PLUGIN_LUA_NAME}
    ${PLUGIN_LUA_TARGET_COMMON_FILES}
    ${PLUGIN_LUA_TARGET_SRC_FILES}
    ${PLUGIN_LUA_TARGET_INC_FILES}
)

# Engine include path
target_include_directories(${PLUGIN_LUA_NAME} PUBLIC ${ENGINE_INC_DIR})
# Plugin include path
target_include_directories(${PLUGIN_LUA_NAME} PUBLIC ${PLUGIN_LUA_INC_DIR})

# Link libraries
target_link_libraries(${PLUGIN_LUA_NAME}
    PUBLIC
        ${ENGINE_NAME}
)

# テスト用サブディレクトリを追加
add_subdirectory(Test)
