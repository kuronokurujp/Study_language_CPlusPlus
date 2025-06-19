# プラグインビルドの共通処理.
include(${CMAKE_CURRENT_SOURCE_DIR}/../Plugin.cmake NO_POLICY_SCOPE)
# プラグインの共通情報.
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICY_SCOPE)
# 共有ライブラリとして生成するためのオプションの定義.
option(PLATFORMSDL2_PLUGIN_BUILD_SHARED_LIBS "build shared library" OFF)
# プラグイン共通ビルド処理.
common_build_plugin("${PLATFORMSDL2_PLUGIN_NAME}" "${PLATFORMSDL2_PLUGIN_TARGET_FILES}" "$CACHE{CACHE_ENGINE_INCLUDE_CODE}" ${PLATFORMSDL2_PLUGIN_BUILD_SHARED_LIBS})

# プラグインが利用するパッケージをvcpkgから取得
vcpkg_install_package($CACHE{CHACHE_VCPKG_ROOT} ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}" $CACHE{CACHE_PROJECT_VCPKG_STATIC_LIB_TYPE})

# インストールしたパッケージのディレクトリパス
set(VCPKG_DIR_PATH ThirdParty/VcPkg/$CACHE{CACHE_PROJECT_VCPKG_STATIC_LIB_TYPE})
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

    target_link_libraries(${PLATFORMSDL2_PLUGIN_NAME}
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

    target_include_directories(${PLATFORMSDL2_PLUGIN_NAME}
        PRIVATE
            # 絶対パスの中にincludeの文字列が入っているとパス設定がうまくいかない
            # 相対パス設定ならうまくいった
            ./${VCPKG_DIR_PATH}/include/
    )
else()
    message(STATUS "Non VcpkgDir ${VCPKG_DIR_PATH}")
endif()

