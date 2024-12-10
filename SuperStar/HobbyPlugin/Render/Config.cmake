set(RENDER_PLUGIN_NAME HobbyEnginePlugin_Render)

# プロジェクトに登録するファイル群
# ここに全て記載する
set(RENDER_PLUGIN_TARGET_FILES
    .editorconfig
    LICENSE.txt
    README.md

    RenderModule.cpp
    RenderModule.h
    
    Render/Color.h

    Render/Command/CommandBuffer.h
    Render/Command/Command.h
    Render/Command/Command.cpp

    Render/Window/Window.cpp
    Render/Window/Window.h
    Render/Window/ViewPort.h
    Render/Window/ViewPort.cpp
    Render/Window/Scene.h
    Render/Window/Scene.cpp
    
    Render/Draw/Mesh.h
    Render/Draw/Mesh.cpp
    Render/Draw/Material.h
    Render/Draw/Material.cpp
)
