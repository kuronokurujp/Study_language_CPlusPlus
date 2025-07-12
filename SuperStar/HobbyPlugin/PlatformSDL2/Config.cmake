set(PLUGIN_PLATFORMSDL2_NAME HobbyPlugin_Platform_SDL2)
set(PLUGIN_PLATFORMSDL2_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/Inc)

set(PLUGIN_PLATFORMSDL2_TARGET_COMMON_FILES
    .editorconfig
    LICENSE.txt
    README.md
    ThirdPartyNotices.txt
)

set(PLUGIN_PLATFORMSDL2_TARGET_INC_FILES
    Inc/PlatformSDL2Module.h

    Inc/PlatformSDL2/SDL2File.h
    Inc/PlatformSDL2/SDL2Font.h
    Inc/PlatformSDL2/SDL2Input.h
    Inc/PlatformSDL2/SDL2Graphic.h
    Inc/PlatformSDL2/SDL2System.h
    Inc/PlatformSDL2/SDL2Time.h

    Inc/PlatformSDL2/Graphic/Frame.h
    Inc/PlatformSDL2/Graphic/Drawable2D.h

    Inc/PlatformSDL2/Graphic/Draw/Material.h
    Inc/PlatformSDL2/Graphic/Draw/Mesh.h
    Inc/PlatformSDL2/Graphic/Draw/Texture.h
)

set(PLUGIN_PLATFORMSDL2_TARGET_SRC_FILES
    Src/PlatformSDL2Module.cpp

    Src/SDL2File.cpp
    Src/SDL2Font.cpp
    Src/SDL2Input.cpp
    Src/SDL2Graphic.cpp
    Src/SDL2System.cpp
    Src/SDL2Time.cpp

    Src/Graphic/Frame.cpp
    Src/Graphic/Drawable2D.cpp
    Src/Graphic/Draw/Material.cpp
    Src/Graphic/Draw/Mesh.cpp
    Src/Graphic/Draw/Texture.cpp
)

function(setup_sdl2)
    # SDL2のプラットフォームタイプをマクロ設定
    message(Platform Type: HE_USE_SDL2)
    add_compile_definitions(HE_USE_SDL2)
endfunction()

# includeと同時に実行
setup_sdl2()
