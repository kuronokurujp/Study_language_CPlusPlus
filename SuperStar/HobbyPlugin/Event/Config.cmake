set(PLUGIN_EVENT_NAME HobbyPlugin_Event)
set(PLUGIN_EVENT_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/Inc)

set(PLUGIN_EVENT_TARGET_COMMON_FILES
    .editorconfig
    LICENSE.txt
    README.md

)
set(PLUGIN_EVENT_TARGET_INC_FILES
    Inc/EventModule.h

    Inc/Event/EventData.h
    Inc/Event/EventInterface.h
    Inc/Event/EventManager.h
)

set(PLUGIN_EVENT_TARGET_SRC_FILES
    Src/EventModule.cpp
    Src/EventManager.cpp
)

