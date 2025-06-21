set(PLUGIN_ACTOR_NAME HobbyPlugin_Actor)
set(PLUGIN_ACTOR_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/Inc)

set(PLUGIN_ACTOR_TARGET_COMMON_FILES
    .editorconfig
    LICENSE.txt
    README.md

)
set(PLUGIN_ACTOR_TARGET_INC_FILES
    Inc/ActorModule.h

    Inc/Actor/Actor.h
    Inc/Actor/ActorInterface.h
    Inc/Actor/ActorManager.h

    Inc/Actor/Component/Component.h
    Inc/Actor/Component/TransformComponent.h
)

set(PLUGIN_ACTOR_TARGET_SRC_FILES
    Src/ActorModule.cpp

    Src/Actor.cpp
    Src/ActorManager.cpp
    Src/Component/Component.cpp
    Src/Component/TransformComponent.cpp
)
