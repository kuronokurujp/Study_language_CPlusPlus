# プロジェクト全体で利用する基本となる文字コードをUTF8にする
# SDL2はwchar_t型を使わないから
set(CACHE_CHARACTER_CODE_UTF8_FLAG 1 CACHE STRING "" FORCE)

set(PLATFORMSDL2_PLUGIN_NAME HobbyEnginePlugin_PlatformSDL2)
# プロジェクトに登録するファイル群
# ここに全て記載する
set(PLATFORMSDL2_PLUGIN_TARGET_FILES
    .editorconfig
    LICENSE.txt
    README.md
    PlatformSDL2Module.cpp
    PlatformSDL2Module.h
    
    PlatformSDL2/SDL2File.cpp
    PlatformSDL2/SDL2File.h
    PlatformSDL2/SDL2Input.cpp
    PlatformSDL2/SDL2Input.h
    PlatformSDL2/SDL2Time.cpp
    PlatformSDL2/SDL2Time.h
    PlatformSDL2/SDL2Screen.h
    PlatformSDL2/SDL2Screen.cpp
    PlatformSDL2/SDL2System.h
    PlatformSDL2/SDL2System.cpp
    PlatformSDL2/SDL2Font.cpp
    PlatformSDL2/SDL2Font.h

    PlatformSDL2/Screen/Window.cpp
    PlatformSDL2/Screen/Window.h
    PlatformSDL2/Screen/ViewPort.h
    PlatformSDL2/Screen/ViewPort.cpp
    PlatformSDL2/Screen/Scene.h
    PlatformSDL2/Screen/Scene.cpp
    
    PlatformSDL2/Screen/Draw/Material.cpp
    PlatformSDL2/Screen/Draw/Material.h
    PlatformSDL2/Screen/Draw/Mesh.cpp
    PlatformSDL2/Screen/Draw/Mesh.h
    PlatformSDL2/Screen/Draw/Texture.cpp
    PlatformSDL2/Screen/Draw/Texture.h
)
