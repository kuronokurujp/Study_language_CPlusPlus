set(ASSET_MANAGER_PLUGIN_NAME HobbyEnginePlugin_AssetManager)

# プロジェクトに登録するファイル群
# ここに全て記載する
set(ASSET_MANAGER_PLUGIN_TARGET_FILES
    .editorconfig
    LICENSE.txt
    README.md

    AssetManagerModule.cpp
    AssetManagerModule.h
    
    AssetManager/AssetDataBase.h
    AssetManager/AssetDataBase.cpp
    AssetManager/AssetDataJson.h
    AssetManager/AssetDataJson.cpp
    AssetManager/AssetDataToml.h
    AssetManager/AssetDataToml.cpp
    AssetManager/AssetDataXml.h
    AssetManager/AssetDataXml.cpp
    AssetManager/AssetDataBinary.h 
    AssetManager/AssetDataBinary.cpp
    AssetManager/AssetDataText.h 
    AssetManager/AssetDataText.cpp

    AssetManager/ThirdParty/pugixml/pugiconfig.hpp
    AssetManager/ThirdParty/pugixml/pugixml.cpp
    AssetManager/ThirdParty/pugixml/pugixml.hpp
    AssetManager/ThirdParty/simidjson/simdjson.h
    AssetManager/ThirdParty/simidjson/simdjson.cpp
    AssetManager/ThirdParty/tomlplusplus/toml.hpp
)
