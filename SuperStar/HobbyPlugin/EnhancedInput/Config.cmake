set(PLUGIN_ENHANCED_INPUT_NAME HobbyPlugin_EnhancedInput)
set(PLUGIN_ENHANCED_INPUT_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/Inc)

set(PLUGIN_ENHANCED_INPUT_TARGET_COMMON_FILES
    .editorconfig
    LICENSE.txt
    README.md
)

set(PLUGIN_ENHANCED_INPUT_TARGET_INC_FILES
    Inc/EnhancedInputModule.h

    Inc/EnhancedInput/Common.h
    Inc/EnhancedInput/Event.h

    Inc/EnhancedInput/Actor/ActorManagerDecorater.h
    Inc/EnhancedInput/Actor/InputComponent.h
)

set(PLUGIN_ENHANCED_INPUT_TARGET_SRC_FILES
    Src/EnhancedInputModule.cpp
    Src/Actor/ActorManagerDecorater.cpp
    Src/Actor/InputComponent.cpp
)
