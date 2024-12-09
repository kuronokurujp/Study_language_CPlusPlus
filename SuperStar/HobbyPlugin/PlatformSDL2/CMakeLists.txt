# プラグインビルドの共通処理.
include(${CMAKE_CURRENT_SOURCE_DIR}/../Plugin.cmake NO_POLICTY_SCOPE)
# プラグインの共通情報.
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICTY_SCOPE)
# 共有ライブラリとして生成するためのオプションの定義.
option(PLATFORMSDL2_PLUGIN_BUILD_SHARED_LIBS "build shared library" OFF)
# プラグイン共通ビルド処理.
common_build_plugin("${PLATFORMSDL2_PLUGIN_NAME}" "${PLATFORMSDL2_PLUGIN_TARGET_FILES}" "$CACHE{CACHE_ENGINE_INCLUDE_CODE}" ${PLATFORMSDL2_PLUGIN_BUILD_SHARED_LIBS})

# TODO: プラグインが利用するパッケージをvcpkgから取得
install_vcpkg_package($CACHE{CHACHE_VCPKG_ROOT} ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/VcPkg" $CACHE{CACHE_PROJECT_VCPKG_STATIC_LIB_TYPE})

# インストールしたパッケージのディレクトリパス
set(VCPKG_DIR_PATH VcPkg/$CACHE{CACHE_PROJECT_VCPKG_STATIC_LIB_TYPE})
# TODO: プラグイン用のパッケージディレクトリ
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${VCPKG_DIR_PATH}/share/sdl2")

# TODO: SDL2をvcpkgでインストール
find_package(SDL2 REQUIRED CONFIG)

target_link_libraries(${PLATFORMSDL2_PLUGIN_NAME} 
    PRIVATE
    # TODO: SDL2関連のライブラリを設定
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

target_include_directories(${PLATFORMSDL2_PLUGIN_NAME} 
    PRIVATE
        # 絶対パスの中にincludeの文字列が入っているとパス設定がうまくいかない
        # 相対パス設定ならうまくいった
        ./${VCPKG_DIR_PATH}/include/
        # プラグインに依存しているプラグインコード
        # ライブラリのリンクはcmakeで指定したCMakeLists.txtでやっている
        # 相対パスだと自動で${CMAKE_CURRENT_SOURCE_DIR}が追加される
        ../Render/
)
