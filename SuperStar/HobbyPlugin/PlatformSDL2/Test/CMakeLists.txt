cmake_minimum_required(VERSION 3.26)

include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/VCPkg.cmake NO_POLICY_SCOPE)
include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/Project.cmake NO_POLICY_SCOPE)
include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/Test.cmake NO_POLICY_SCOPE)

include(${CMAKE_CURRENT_SOURCE_DIR}/../Config.cmake NO_POLICY_SCOPE)
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICY_SCOPE)

add_executable(${PLATFORMSDL2_PLUGIN_TEST_NAME}
    ${PLATFORMSDL2_PLUGIN_TARGET_TEST_SRC_FILES}
)

project_configure_target(${PLATFORMSDL2_PLUGIN_TEST_NAME})

project_configure_files_target(${PLATFORMSDL2_PLUGIN_TEST_NAME}
    ${PLATFORMSDL2_PLUGIN_TARGET_TEST_SRC_FILES}
)

vcpkg_install_package(${CMAKE_CURRENT_SOURCE_DIR} "x64-windows-static")

test_configure_target(
    ${PLATFORMSDL2_PLUGIN_TEST_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/VcPkg/x64-windows-static
    ${PLATFORMSDL2_PLUGIN_NAME}
)
