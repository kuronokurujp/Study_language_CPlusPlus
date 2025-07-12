# テスト専用のプロジェクト設定
function(test_configure_target target vcpkg_dir)
    # ARGN に渡されたすべての引数（ライブラリ一覧）が入る
    set(target_test_libs ${ARGN})

    # vcpkg のパッケージディレクトリがあれば検索パスに追加
    if(EXISTS ${vcpkg_dir})
        list(APPEND CMAKE_PREFIX_PATH "${vcpkg_dir}")
    endif()

    # まず find_package で gtest を探す
    find_package(GTest CONFIG QUIET)

    if(GTest_FOUND)
        enable_testing()
        target_include_directories(${target} PRIVATE ${GTEST_INCLUDE_DIRS})
        target_link_libraries(${target} PRIVATE GTest::gtest_main ${target_test_libs})
    else()
        # ローカルに gtest がある場合はサブディレクトリとして追加
        if(NOT TARGET gtest)
            add_subdirectory(${CMAKE_SOURCE_DIR}/ThirdParty/googletest)
        endif()
        enable_testing()
        target_include_directories(${target} PRIVATE
            ${CMAKE_SOURCE_DIR}/ThirdParty/googletest/googletest/include)
        target_link_libraries(${target} PRIVATE gtest_main gtest ${target_test_libs})
    endif()

    # テスト実行ファイルの出力先をTestフォルダに設定
    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Bin/Test/$<CONFIG>")

    # CTest に登録
    add_test(NAME ${target} COMMAND ${target})
endfunction()
