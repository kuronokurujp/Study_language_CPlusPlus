# プラグインの共通処理を記述

# プラグインの共通ビルド処理
function(common_build_plugin name files engine_includes bShared)
    # オプションで生成するライブラリの種類(SHARED/STATIC)を切り替える.
    if (${bShared})
        add_library(${name} SHARED)
    else()
        add_library(${name} STATIC)
    endif()

    # C++のコンパイラの設定
    set_target_properties(${name} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
    )

    # 生成に必要なソースファイル.
    target_sources(${name}
        PRIVATE
        ${files}
    )

    # プロジェクトが参照できるインクルードディレクトリのパス.
    # リンク側は参照してほしくない
    target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/)
    # リンク側で参照してほしい
    target_include_directories(${name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

    target_include_directories(${name}
        PRIVATE
            # Engineのインクルードディレクトリ.
            ${engine_includes}
    )

    # ランタイムライブラリを設定
    target_compile_options(
        ${name}
        PRIVATE
        $<IF:$<CONFIG:Debug>,/MTd,/MT>
    )

    # ビルドのマルチプロセス設定.
    target_compile_options(${name} PRIVATE /MP)
    # Unicode文字セットを設定.
    target_compile_options(${name} PRIVATE /DUNICODE /D_UNICODE)

    # ファイルのフィルタリング設定.
    # VSプロジェクト上で見やすくするため.
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${files})
endfunction()

# VCPKGのライブラリをインストール
function(install_vcpkg_package vcpkg_dir_path vcpkg_manifest_root_path vcpkg_install_dir_path vcpkg_triplet)

# 一旦vcpkg関連のファイルを削除
# これはvcpkgを入れなおしたら環境再構築するため
if(EXISTS ${vcpkg_manifest_root_path}/vcpkg-configuration.json)
    file(REMOVE ${vcpkg_manifest_root_path}/vcpkg-configuration.json)
endif()

if(EXISTS ${vcpkg_manifest_root_path}/vcpkg.json)
    file(REMOVE ${vcpkg_manifest_root_path}/vcpkg.json)
endif()

# vcpkgの環境を構築
execute_process(
    COMMAND cmd /c "${vcpkg_dir_path}/vcpkg.exe" "--vcpkg-root=${vcpkg_dir_path}" new --application
    WORKING_DIRECTORY ${vcpkg_manifest_root_path}
    OUTPUT_VARIABLE CMD_OUTPUT
    ERROR_VARIABLE CMD_ERROR
    RESULT_VARIABLE CMD_RESULT
)

# プラグインが用意した.vcpkg.jsonをvcpkg.jsonにコピー
if(EXISTS ${vcpkg_manifest_root_path}/.vcpkg.json)
    configure_file(${vcpkg_manifest_root_path}/.vcpkg.json ${vcpkg_manifest_root_path}/vcpkg.json COPYONLY)
endif()

execute_process(
     COMMAND cmd /c "${vcpkg_dir_path}/vcpkg.exe" "--vcpkg-root=${vcpkg_dir_path}" install --clean-after-build "--x-manifest-root=${vcpkg_manifest_root_path}" "--x-install-root=${vcpkg_install_dir_path}" "--triplet=${vcpkg_triplet}"
     WORKING_DIRECTORY ${vcpkg_manifest_root_path}
     OUTPUT_VARIABLE CMD_OUTPUT
     ERROR_VARIABLE CMD_ERROR
     RESULT_VARIABLE CMD_RESULT
 )

message(STATUS "Command Output: ${CMD_OUTPUT}")
message(STATUS "Command Error: ${CMD_ERROR}")
message(STATUS "Command Result: ${CMD_RESULT}")

endfunction()