set(PLUGIN_ASSET_MANAGER_NAME HobbyPlugin_AssetManager)
set(PLUGIN_ASSET_MANAGER_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/Inc)
set(PLUGIN_ASSET_MANAGER_THIRD_PARTY_DIR ${CMAKE_CURRENT_LIST_DIR}/ThirdParty)

set(PLUGIN_ASSET_MANAGER_TARGET_COMMON_FILES
    .editorconfig
    LICENSE.txt
    README.md
)

set(PLUGIN_ASSET_MANAGER_TARGET_INC_FILES
    Inc/AssetManagerModule.h

    Inc/AssetManager/AssetDataBase.h
    Inc/AssetManager/AssetDataJson.h
    Inc/AssetManager/AssetDataToml.h
    Inc/AssetManager/AssetDataXml.h
    Inc/AssetManager/AssetDataBinary.h
    Inc/AssetManager/AssetDataText.h
)


set(PLUGIN_ASSET_MANAGER_TARGET_SRC_FILES
    Src/AssetManagerModule.cpp

    Src/AssetDataBase.cpp
    Src/AssetDataJson.cpp
    Src/AssetDataToml.cpp
    Src/AssetDataXml.cpp
    Src/AssetDataBinary.cpp
    Src/AssetDataText.cpp

    # ThirdPartyの機能は外部公開したくないのでSRCファイルリストに
    ThirdParty/pugixml/pugiconfig.hpp
    ThirdParty/pugixml/pugixml.cpp
    ThirdParty/pugixml/pugixml.hpp
    ThirdParty/simidjson/simdjson.h
    ThirdParty/simidjson/simdjson.cpp
    ThirdParty/tomlplusplus/toml.hpp
)
