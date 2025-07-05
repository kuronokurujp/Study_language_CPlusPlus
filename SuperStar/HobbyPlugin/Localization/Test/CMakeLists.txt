cmake_minimum_required(VERSION 3.26)

# Engine common cmake
include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/VCPkg.cmake NO_POLICY_SCOPE)
include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/Project.cmake NO_POLICY_SCOPE)
include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/Test.cmake NO_POLICY_SCOPE)
include(${CMAKE_SOURCE_DIR}/HobbyPlugin/PlatformSDL2/Config.cmake NO_POLICY_SCOPE)

include(${CMAKE_CURRENT_SOURCE_DIR}/../Config.cmake NO_POLICY_SCOPE)
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICY_SCOPE)

add_executable(${LOCALIZATION_PLUGIN_TEST_NAME}
    ${LOCALIZATION_PLUGIN_TARGET_TEST_SRC_FILES}
)

project_configure_target(${LOCALIZATION_PLUGIN_TEST_NAME})

project_configure_files_target(${LOCALIZATION_PLUGIN_TEST_NAME}
    ${LOCALIZATION_PLUGIN_TARGET_TEST_SRC_FILES}
)

vcpkg_install_package(${CMAKE_CURRENT_SOURCE_DIR} "x64-windows-static")

test_configure_target(
    ${LOCALIZATION_PLUGIN_TEST_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/VcPkg/x64-windows-static
    ${ENGINE_NAME}
    ${LOCALIZATION_PLUGIN_NAME}
    ${PLUGIN_PLATFORMSDL2_NAME}
)
