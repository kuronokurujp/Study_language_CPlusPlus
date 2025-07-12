# プロジェクト構築の共通処理

# ターゲットへプロジェクトの設定
function(project_configure_target target)
    # C++ 標準
    set_target_properties(${target} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
    )

    # ランタイムライブラリを設定
    # 統一しないとライブラリ結合で問題が起きる
    if(MSVC)
        target_compile_options(
            ${target}
            PRIVATE
            $<IF:$<CONFIG:Debug>,/MTd,/MT>
        )
    endif()

    # マルチプロセスビルド
    target_compile_options(${target} PRIVATE /MP)
    # Unicode 設定
    target_compile_definitions(${target} PRIVATE UNICODE _UNICODE)
endfunction()

# ターゲットへ共有ライブラリ(dll)の設定
function(project_configure_shared_lib_target target)
    add_library(${target} SHARED)
endfunction()

# ターゲットへ静的ライブラリ(lig)の設定
function(project_configure_static_lib_target target)
    add_library(${target} STATIC)
endfunction()

# ターゲットへファイル群の設定
function(project_configure_files_target target)
    # ARGN に渡されたすべての引数（ファイル一覧）が入る
    set(files ${ARGN})

    # engine生成に必要なソースファイル.
    target_sources(${target}
        PRIVATE
        ${files}
    )

    # ファイルのフィルタリング設定.
    # VSプロジェクト上で見やすくするため.
    if (MSVC)
        source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${files})
    endif()
endfunction()

