cmake_minimum_required(VERSION 3.26)

include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/VCPkg.cmake NO_POLICY_SCOPE)
include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/Project.cmake NO_POLICY_SCOPE)
include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/Test.cmake NO_POLICY_SCOPE)
include(${CMAKE_SOURCE_DIR}/HobbyPlugin/AssetManager/Config.cmake NO_POLICY_SCOPE)
include(${CMAKE_SOURCE_DIR}/HobbyPlugin/PlatformSDL2/Config.cmake NO_POLICY_SCOPE)

include(${CMAKE_CURRENT_SOURCE_DIR}/../Config.cmake NO_POLICY_SCOPE)
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICY_SCOPE)

add_executable(${PLUGIN_LOCALIZATION_TEST_NAME}
    ${PLUGIN_LOCALIZATION_TARGET_TEST_SRC_FILES}
)

project_configure_target(${PLUGIN_LOCALIZATION_TEST_NAME})

project_configure_files_target(${PLUGIN_LOCALIZATION_TEST_NAME}
    ${PLUGIN_LOCALIZATION_TARGET_TEST_SRC_FILES}
)

vcpkg_install_package(${CMAKE_CURRENT_SOURCE_DIR} "x64-windows-static")

test_configure_target(
    ${PLUGIN_LOCALIZATION_TEST_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/VcPkg/x64-windows-static
    ${PLUGIN_ASSET_MANAGER_NAME}
    ${PLUGIN_PLATFORMSDL2_NAME}
    ${LOCALIZATION_PLUGIN_NAME}
)

add_custom_command(
    TARGET ${PLUGIN_LOCALIZATION_TEST_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
            "$<TARGET_FILE_DIR:${PLUGIN_LOCALIZATION_TEST_NAME}>/Resources"
    COMMENT "Copying Resources to output dir"
)
