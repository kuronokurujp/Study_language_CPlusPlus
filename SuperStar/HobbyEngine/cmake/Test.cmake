# gtest を利用したテストの環境設定
function(test_gtest_env vcpkg_dir)
    # ARGN に渡されたすべての引数（ライブラリ一覧）が入る
    set(target_test_libs ${ARGN})

    # インストールしたパッケージのディレクトリパス
    if(EXISTS ${vcpkg_dir})
        # プラグイン用のパッケージディレクトリを設定
        list(APPEND CMAKE_PREFIX_PATH "${vcpkg_dir}")

        # GTestのパッケージを参照
        find_package(GTest CONFIG REQUIRED)
    else()
        message(STATUS "Non VcpkgDir ${vcpkg_dir}")
    endif()
endfunction()

# テスト専用のプロジェクト設定
function(test_configure_target target)
    # ARGN に渡されたすべての引数（ライブラリ一覧）が入る
    set(target_test_libs ${ARGN})

    # GTestのパッケージを参照
    find_package(GTest CONFIG)

    # インストールしたパッケージのディレクトリパス
    if(GTest_FOUND)
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

        # テスト実行ファイルの出力先をTestフォルダに設定
        set_target_properties(${target} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY
                "${CMAKE_SOURCE_DIR}/Bin/Test/$<CONFIG>"
        )

        # CTest に登録
        add_test(NAME ${target} COMMAND ${target})
    else()
        message(STATUS "GTest not found, skipping test configuration for ${target}")
    endif()
endfunction()
