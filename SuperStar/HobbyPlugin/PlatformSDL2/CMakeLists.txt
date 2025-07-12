cmake_minimum_required(VERSION 3.26)

# Engine common cmake
include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/Project.cmake NO_POLICY_SCOPE)
# VCPkgの設定
include(${CMAKE_SOURCE_DIR}/HobbyEngine/cmake/VCPkg.cmake NO_POLICY_SCOPE)
# エンジン設定
include(${CMAKE_SOURCE_DIR}/HobbyEngine/Config.cmake NO_POLICY_SCOPE)
# Plugin common info
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICY_SCOPE)

message("build PlatformSDL2 plugin lib!")

# Static library target
project_configure_static_lib_target(${PLUGIN_PLATFORMSDL2_NAME})

# Project configuration
project_configure_target(${PLUGIN_PLATFORMSDL2_NAME})

# Files for the target
project_configure_files_target(${PLUGIN_PLATFORMSDL2_NAME}
    ${PLUGIN_PLATFORMSDL2_TARGET_COMMON_FILES}
    ${PLUGIN_PLATFORMSDL2_TARGET_SRC_FILES}
    ${PLUGIN_PLATFORMSDL2_TARGET_INC_FILES}
)

# プラグインが利用するパッケージをvcpkgから取得
vcpkg_install_package(${CMAKE_CURRENT_SOURCE_DIR} "x64-windows-static")

# インストールしたパッケージのディレクトリパス
set(VCPKG_DIR_PATH ThirdParty/VcPkg/x64-windows-static)
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${VCPKG_DIR_PATH})
    # プラグイン用のパッケージディレクトリ
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${VCPKG_DIR_PATH}")

    # freetype
    find_package(Freetype REQUIRED)
    # freetypeを使う上で必要なライブラリ
    find_package(PNG REQUIRED)
    find_package(ZLIB REQUIRED)
    find_package(BZip2 REQUIRED)
    find_package(unofficial-brotli CONFIG REQUIRED)

    # SDL2をvcpkgでインストール
    find_package(SDL2 REQUIRED CONFIG)
    # GLEWをvcpkeでインストール
    find_package(GLEW REQUIRED CONFIG)
    # SDL2-ttf(freetypeのライブラリが必要)
    find_package(SDL2_ttf REQUIRED CONFIG)
    # SOIL2
    find_package(SOIL2 REQUIRED)

    target_link_libraries(${PLUGIN_PLATFORMSDL2_NAME}
        PRIVATE
        # SDL2関連のライブラリを設定
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        # libpngのライブラリ
        PNG::PNG
        # Zlibのライブラリ
        ZLIB::ZLIB
        # BZip2のライブラリ
        BZip2::BZip2
        # Brotliのライブラリ
        unofficial::brotli::brotlienc
        unofficial::brotli::brotlidec
        # freetypeのライブラリ
        Freetype::Freetype
        # SDL2-ttf関連のライブラリを設定
        $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
        # GLEWを設定
        GLEW::GLEW
        # SOIL2
        # 他と違ってパス連結しないといけない
        ${SOIL2_INCLUDE_DIRS}/../lib/soil2.lib
    )

    target_link_libraries(${PLUGIN_PLATFORMSDL2_NAME}
        PUBLIC
            # エンジンのライブラリをリンク
            ${ENGINE_NAME}
    )

    target_include_directories(${PLUGIN_PLATFORMSDL2_NAME}
        PRIVATE
            # 絶対パスの中にincludeの文字列が入っているとパス設定がうまくいかない
            # 相対パス設定ならうまくいった
            ./${VCPKG_DIR_PATH}/include/
    )

    target_include_directories(${PLUGIN_PLATFORMSDL2_NAME} PUBLIC ${PLUGIN_PLATFORMSDL2_INC_DIR})
else()
    message(STATUS "Non VcpkgDir ${VCPKG_DIR_PATH}")
endif()

add_subdirectory(Test)

