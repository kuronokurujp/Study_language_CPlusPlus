set(GAMEDEVGUI_PLUGIN_NAME HobbyEnginePlugin_GameDevGUI)
# プロジェクトに登録するファイル群
# ここに全て記載する
set(GAMEDEVGUI_PLUGIN_TARGET_FILES
    .editorconfig
    LICENSE.txt
    README.md
    GameDevGUIModule.cpp
    GameDevGUIModule.h
    
    GameDevGUI/Screen/Scene.cpp
    GameDevGUI/Screen/Scene.h
    GameDevGUI/Screen/Window.cpp
    GameDevGUI/Screen/Window.h
    
    GameDevGUI/PlatformScreen.h
    GameDevGUI/PlatformScreen.cpp
)
