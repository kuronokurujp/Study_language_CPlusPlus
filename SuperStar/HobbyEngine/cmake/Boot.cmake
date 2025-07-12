# ルートCMakeLilst.txtで呼び出すブート処理

# ルートのCMakeLists.txtのブート設定
function(boot_configure)
    # C++ 標準
    # Suppress generation of ZERO_CHECK project.
    set(CMAKE_SUPPRESS_REGENERATION ON)

    # makeコマンド実行時のビルドステップの詳細表示.
    set(CMAKE_VERBOSE_MAKEFILE ON)

    # 警告とすべてエラーとして扱う.
    set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

    # C++コンパイラの設定.
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    # Cコンパイラの設定.
    set(CMAKE_C_STANDARD 17)
    set(CMAKE_C_STANDARD_REQUIRED ON)

    # MSVCコンパイラだとCPPのバージョン定義は以下のオプションがついていないといけない
    if(MSVC)
        add_compile_options(/Zc:__cplusplus)
    endif()

    # 各ビルドタイプに対応するフラグを設定する
    # set(CMAKE_CXX_FLAGS_UNITTEST "${CMAKE_CXX_FLAGS_DEBUG}")
    # set(CMAKE_C_FLAGS_UNITTEST "${CMAKE_C_FLAGS_DEBUG}")
    # set(CMAKE_EXE_LINKER_FLAGS_UNITTEST "")
    # set(CMAKE_SHARED_LINKER_FLAGS_UNITTEST "")

    # ランタイムライブラリを /MT または /MD に統一
    if(MSVC)
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    endif()

    # アーキテクチャ指定.
    if(MSVC)
        if(CMAKE_EXE_LINKER_FLAGS STREQUAL "/machine:x64")
            set(ARCH x86_64)
            set(DX_ARCH x64)
        elseif(CMAKE_EXE_LINKER_FLAGS STREQUAL "/machine:X86")
            set(ARCH x86)
            set(DX_ARCH Win32)
        else()
            message(FATAL_ERROR "[${WORKSPACE_NAME}] Invalid linker flag ${CMAKE_EXE_LINKER_FLAGS}.")
        endif()
    else()
        # For non-MSVC toolchains derive the architecture from the processor.
        set(ARCH "${CMAKE_SYSTEM_PROCESSOR}")
    endif()

    # VSコンパイラを指定.
    if(MSVC)
        if(MSVC_VERSION GREATER_EQUAL 1930 AND MSVC_VERSION LESS 1950)
            # Visual Studio 2022
            set(COMPILER 143)
            set(MSVC_YEAR 2022)
        else()
            message(FATAL_ERROR "[${WORKSPACE_NAME}] Unsupported Visual C++ compiler used.")
        endif()
    endif()
endfunction()

# includeと同時に実行
boot_configure()
