# プロジェクト名を設定
set(APP_NAME Game)

# プロジェクトに登録するファイル群
# ここに全て記載する
set(TARGET_FILES
	.editorconfig
	README.md

#	Code/Common.cpp
#	Code/Common.h

	Code/Main.cpp
	Code/Main.h

#	Code/Asset/ParamaterAssetData.h
#	Code/Asset/ParamaterAssetData.cpp

#	Code/Level/LevelLauncher.cpp
#	Code/Level/LevelLauncher.h
#	Code/Level/LevelTitle.cpp
#	Code/Level/LevelTitle.h
#	Code/Level/LevelInGame.cpp
#	Code/Level/LevelInGame.h
#	Code/Level/LevelInGame/LevelInGame_BG.cpp
#	Code/Level/LevelInGame/LevelInGame_BG.h
#
#	Code/InGame/Component/InGameSystemComponent.h
#	Code/InGame/Component/InGameSystemComponent.cpp
#	Code/InGame/Component/InGameBulletManagerComponent.cpp
#	Code/InGame/Component/InGameBulletManagerComponent.h
#	Code/InGame/Component/InGameStageManagerComponent.cpp
#	Code/InGame/Component/InGameStageManagerComponent.h
#	Code/InGame/Component/InGameCollisionComponent.cpp
#	Code/InGame/Component/InGameCollisionComponent.h
#	Code/InGame/Component/InGameShotComponent.cpp
#	Code/InGame/Component/InGameShotComponent.h
#	Code/InGame/Component/Renderer/InGameRendererUserShipComponent.cpp
#	Code/InGame/Component/Renderer/InGameRendererUserShipComponent.h
#	Code/InGame/Component/Renderer/InGameRendererEnemyZakoComponent.cpp
#	Code/InGame/Component/Renderer/InGameRendererEnemyZakoComponent.h
#	Code/InGame/Component/Renderer/InGameRendererBaseComponent.cpp
#	Code/InGame/Component/Renderer/InGameRendererBaseComponent.h
#
#	Code/InGame/Shot/InGameShotStrategy.cpp
#	Code/InGame/Shot/InGameShotStrategy.h
#	Code/InGame/Shot/InGameShotStrategyInterface.h
#
#	Code/InGame/Actor/InGameScene2DActor.cpp
#	Code/InGame/Actor/InGameScene2DActor.h
#
#	Code/InGame/Actor/Player/InGamePlayerActor.cpp
#	Code/InGame/Actor/Player/InGamePlayerActor.h
#
#	Code/InGame/Actor/Enemy/InGameBaseEnemyActor.cpp
#	Code/InGame/Actor/Enemy/InGameBaseEnemyActor.h
#	Code/InGame/Actor/Enemy/InGameSnakeEnemyActor.cpp
#	Code/InGame/Actor/Enemy/InGameSnakeEnemyActor.h
#	Code/InGame/Actor/Enemy/InGameBaseEnemyActor.cpp
#	Code/InGame/Actor/Enemy/InGameBaseEnemyActor.h
#	Code/InGame/Actor/Enemy/InGameSplitEnemyActor.cpp
#	Code/InGame/Actor/Enemy/InGameSplitEnemyActor.h
#	Code/InGame/Actor/Enemy/InGameZakoEnemyActor.cpp
#	Code/InGame/Actor/Enemy/InGameZakoEnemyActor.h
#
#	Code/InGame/Actor/Boss/InGameBoss01Actor.cpp
#	Code/InGame/Actor/Boss/InGameBoss01Actor.h
#
#	Code/InGame/Bullet/InGameBulletInterface.h
#	Code/InGame/Bullet/InGameBulletNormal.h
#	Code/InGame/Bullet/InGameBulletNormal.cpp
#
#	Code/InGame/InGameTag.h
#
#	Code/InGame/Event/InGameEventCharacter.h
#	Code/InGame/Event/InGameEventCharacter.cpp
#	Code/InGame/Event/InGameEventShot.h
#	Code/InGame/Event/InGameEventShot.cpp
#
#	Code/Platform/Win/Framework.h
#	Code/Platform/Win/Resource.h
#	Code/Platform/Win/Targetver.h
#
#	Resource/Assets/Locate/JP/Debug.json
#	Resource/Assets/Locate/JP/Text.json
#	Resource/Assets/Locate/System.toml
#	Resource/Assets/UI/Builder/Test/Launcher.xml
#	Resource/Assets/UI/Builder/Game/Title.xml
#	Resource/Assets/Lua/Enemy/Zako_001.lua
#	Resource/Assets/Paramater/Enemy.json
#	Resource/Assets/Paramater/Player.json
)
