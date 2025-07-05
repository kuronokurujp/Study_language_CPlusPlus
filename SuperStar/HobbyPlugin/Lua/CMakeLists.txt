cmake_minimum_required(VERSION 3.26)

# プラグインビルドの共通処理.
include(${CMAKE_CURRENT_SOURCE_DIR}/../Plugin.cmake NO_POLICY_SCOPE)
# プラグインの共通情報.
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICY_SCOPE)
# 共有ライブラリとして生成するためのオプションの定義.
option(LUA_PLUGIN_BUILD_SHARED_LIBS "build shared library" OFF)
# プラグイン共通ビルド処理.
common_build_plugin("${LUA_PLUGIN_NAME}" "${LUA_PLUGIN_TARGET_FILES}" "$CACHE{CACHE_ENGINE_INCLUDE_CODE}" ${LUA_PLUGIN_BUILD_SHARED_LIBS})

# プラグインのインクルードディレクトリを公開
target_include_directories(${LUA_PLUGIN_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Inc/Lua
)

# プラグインが利用するパッケージをvcpkgから取得
vcpkg_install_package($CACHE{CHACHE_VCPKG_ROOT} ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}" $CACHE{CACHE_PROJECT_VCPKG_STATIC_LIB_TYPE})

# インストールしたパッケージのディレクトリパス
set(VCPKG_DIR_PATH ThirdParty/VcPkg/$CACHE{CACHE_PROJECT_VCPKG_STATIC_LIB_TYPE})

if(EXISTS ${VCPKG_DIR_PATH})
    # vcpkgの取り扱い設定処理
    # 使う場合はコメントを外して書き換えてください

    # プラグイン用のパッケージディレクトリを設定
    # list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${VCPKG_DIR_PATH}/share/インストールしたパッケージ名のディレクトリ")
    # SDL2のパッケージを探す
    # find_package(インストールしたパッケージ名 REQUIRED CONFIG)

    # プラグインをリンク
    # target_link_libraries(${__MODULE_UPPER_NAME___PLUGIN_NAME}
    #     PRIVATE
    #     # SDL2関連のライブラリを設定
    #     vcpkgのパッケージのライブラリファイルを指定
    # )
    # プラグインのインクルード
    # target_include_directories(${__MODULE_UPPER_NAME___PLUGIN_NAME}
    #     PRIVATE
            # ここは書き換えない
    #         ./${VCPKG_DIR_PATH}/include/
            # 他にもインクルードしたいディレクトリがあれば追加
    # )
else()
    message(STATUS "Non VcpkgDir ${VCPKG_DIR_PATH}")
endif()
