@echo off
chcp 65001

rem アセットデータを更新バッチ

setlocal

REM バッチファイル実行場所によって失敗しないようにパスをバッチファイル場所を基準にする
set SCRIPT_PATH=%~dp0

REM パラメータデータをコンバート
%SCRIPT_PATH%\Tool\GameDataMaker\20241121_180832\main.exe conv-param %SCRIPT_PATH%\Game\Resource\Assets\Paramater %SCRIPT_PATH%\Game\Resource\Orginal\Paramater\Game.xlsm
%SCRIPT_PATH%\Tool\GameDataMaker\20241121_180832\main.exe conv-param %SCRIPT_PATH%\Game\Resource\TestAssets\Paramater %SCRIPT_PATH%\Game\Resource\Orginal\Paramater\Test.xlsm

REM アセットファイルを読み込みディレクトリに転送
call %SCRIPT_PATH%\HobbyEngine\Script\MoveDir.bat %SCRIPT_PATH%\Game\Resource\Assets\ %SCRIPT_PATH%\Bin\SuperStar\Debug\Assets
if %errorlevel% neq 0 pause & exit /b %errorlevel%

call %SCRIPT_PATH%\HobbyEngine\Script\MoveDir.bat %SCRIPT_PATH%\Game\Resource\Assets\ %SCRIPT_PATH%\Bin\SuperStar\Release\Assets
if %errorlevel% neq 0 pause & exit /b %errorlevel%

call %SCRIPT_PATH%\HobbyEngine\Script\MoveDir.bat %SCRIPT_PATH%\Game\Resource\TestAssets\ %SCRIPT_PATH%\Bin\Test\Debug\Assets
if %errorlevel% neq 0 pause & exit /b %errorlevel%

endlocal

REM 正常終了
echo "All Complete" & pause & exit /b 0
