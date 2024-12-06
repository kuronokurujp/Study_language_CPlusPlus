@echo off
chcp 65001

rem アセットデータを更新バッチ

setlocal

REM パラメータデータをコンバート
Tool\GameDataMaker\20241121_180832\main.exe conv-param .\Game\Resource\Assets\Paramater .\Game\Resource\Orginal\Paramater\Game.xlsm
Tool\GameDataMaker\20241121_180832\main.exe conv-param .\Game\Resource\TestAssets\Paramater .\Game\Resource\Orginal\Paramater\Test.xlsm

REM アセットファイルを読み込みディレクトリに転送
call .\HobbyEngine\Script\MoveDir.bat .\Game\Resource\Assets\ .\Bin\SuperStar\Debug\Assets
if %errorlevel% neq 0 pause & exit /b %errorlevel%

call .\HobbyEngine\Script\MoveDir.bat .\Game\Resource\Assets\ .\Bin\SuperStar\Release\Assets
if %errorlevel% neq 0 pause & exit /b %errorlevel%

call .\HobbyEngine\Script\MoveDir.bat .\Game\Resource\TestAssets\ .\Bin\Test\Debug\Assets
if %errorlevel% neq 0 pause & exit /b %errorlevel%

endlocal

REM 正常終了
echo "All Complete" & pause & exit /b 0

