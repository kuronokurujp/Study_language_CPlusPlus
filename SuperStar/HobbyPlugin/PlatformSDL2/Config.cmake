set(PLATFORMSDL2_PLUGIN_NAME HobbyEnginePlugin_PlatformSDL2)
set(PLATFORMSDL2_PLUGIN_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/Inc)

set(PLATFORMSDL2_PLUGIN_TARGET_COMMON_FILES
    .editorconfig
    LICENSE.txt
    README.md
    ThirdPartyNotices.txt
)

set(PLATFORMSDL2_PLUGIN_TARGET_INC_FILES
    Inc/PlatformSDL2Module.h

    Inc/PlatformSDL2/SDL2File.h
    Inc/PlatformSDL2/SDL2Font.h
    Inc/PlatformSDL2/SDL2Input.h
    Inc/PlatformSDL2/SDL2Screen.h
    Inc/PlatformSDL2/SDL2System.h
    Inc/PlatformSDL2/SDL2Time.h

    Inc/PlatformSDL2/Screen/Window.h
    Inc/PlatformSDL2/Screen/ViewPort.h
    Inc/PlatformSDL2/Screen/Scene.h

    Inc/PlatformSDL2/Screen/Draw/Material.h
    Inc/PlatformSDL2/Screen/Draw/Mesh.h
    Inc/PlatformSDL2/Screen/Draw/Texture.h
)

set(PLATFORMSDL2_PLUGIN_TARGET_SRC_FILES
    Src/PlatformSDL2Module.cpp

    Src/PlatformSDL2/SDL2File.cpp
    Src/PlatformSDL2/SDL2Font.cpp
    Src/PlatformSDL2/SDL2Input.cpp
    Src/PlatformSDL2/SDL2Screen.cpp
    Src/PlatformSDL2/SDL2System.cpp
    Src/PlatformSDL2/SDL2Time.cpp

    Src/PlatformSDL2/Screen/Window.cpp
    Src/PlatformSDL2/Screen/ViewPort.cpp
    Src/PlatformSDL2/Screen/Scene.cpp

    Src/PlatformSDL2/Screen/Draw/Material.cpp
    Src/PlatformSDL2/Screen/Draw/Mesh.cpp
    Src/PlatformSDL2/Screen/Draw/Texture.cpp
)

function(setup_sdl2)
    # SDL2のプラットフォームタイプをマクロ設定
    if(DEFINED CACHE{CACHE_PLATFORM_TYPE})
        message(Platform Type: HE_USE_SDL2)
        add_compile_definitions(HE_USE_SDL2)
    else()
        message(None Platform Type)
    endif()
endfunction()

# includeと同時に実行
setup_sdl2()
