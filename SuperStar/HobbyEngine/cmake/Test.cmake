# テスト専用のプロジェクト設定
function(test_congigure_target target vcpkg_dir)
    # ARGN に渡されたすべての引数（ライブラリ一覧）が入る
    set(target_test_libs ${ARGN})

    # インストールしたパッケージのディレクトリパス
    if(EXISTS ${vcpkg_dir})
        # プラグイン用のパッケージディレクトリを設定
        list(APPEND CMAKE_PREFIX_PATH "${vcpkg_dir}")

        # GTestのパッケージを参照
        find_package(GTest CONFIG REQUIRED)
        enable_testing()

        # include ディレクトリに gtest のヘッダを追加
        target_include_directories(${target}
            PRIVATE
                ${GTEST_INCLUDE_DIRS}
        )

        # gtest の main とライブラリ本体、自プロジェクトのライブラリをリンク
        target_link_libraries(${target}
            PRIVATE
                GTest::gtest_main
                ${target_test_libs}
        )

        # CTest に登録
        add_test(NAME ${target} COMMAND ${target})

    else()
        message(STATUS "Non VcpkgDir ${vcpkg_dir}")
    endif()
endfunction()
