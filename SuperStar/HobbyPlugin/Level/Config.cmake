set(PLUGIN_LEVEL_NAME HobbyPlugin_Level)
set(PLUGIN_LEVEL_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/Inc)


set(PLUGIN_LEVEL_TARGET_COMMON_FILES
    .editorconfig
    LICENSE.txt
    README.md
)
set(PLUGIN_LEVEL_TARGET_INC_FILES
    Inc/LevelModule.h
    Inc/Level/LevelNode.h
)

set(PLUGIN_LEVEL_TARGET_SRC_FILES
    Src/LevelModule.cpp
    Src/LevelNode.cpp
)
