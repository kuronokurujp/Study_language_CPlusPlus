cmake_minimum_required(VERSION 3.26)

include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/Project.cmake NO_POLICY_SCOPE)
include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/Test.cmake NO_POLICY_SCOPE)
# TODO: 将来DX12など別プラットフォームを利用するケースがあるので分岐する必要がある
# include(${CMAKE_SOURCE_DIR}/HobbyPlugin/PlatformSDL2/Config.cmake NO_POLICY_SCOPE)

include(${CMAKE_CURRENT_SOURCE_DIR}/../Config.cmake NO_POLICY_SCOPE)
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICY_SCOPE)

add_executable(${PLUGIN_RENDER_TEST_NAME}
    ${PLUGIN_RENDER_TARGET_TEST_SRC_FILES}
)

project_configure_target(${PLUGIN_RENDER_TEST_NAME})

project_configure_files_target(${PLUGIN_RENDER_TEST_NAME}
    ${PLUGIN_RENDER_TARGET_TEST_SRC_FILES}
)

test_configure_target(
    ${PLUGIN_RENDER_TEST_NAME}
    ${PLUGIN_RENDER_NAME}
#	${PLUGIN_PLATFORMSDL2_NAME}
)

# POST_BUILD で Resources フォルダをコピー
add_custom_command(
    TARGET ${PLUGIN_RENDER_TEST_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
            "$<TARGET_FILE_DIR:${PLUGIN_RENDER_TEST_NAME}>/Resources"
    COMMENT "Copying Resources to output dir"
)
