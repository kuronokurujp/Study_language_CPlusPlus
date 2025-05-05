@echo off
chcp 65001

REM アセットデータを更新バッチ

setlocal

REM バッチファイル実行場所によって失敗しないようにパスをバッチファイル場所を基準にする
set SCRIPT_PATH=%~dp0

REM リソースのパス
set RES_PATH=%SCRIPT_PATH%\Game\Resource

REM パラメータデータをコンバート
%SCRIPT_PATH%\Tool\GameDataMaker\20241121_180832\main.exe conv-param %RES_PATH%\Assets\Paramater %RES_PATH%\Orginal\Paramater\Game.xlsm
%SCRIPT_PATH%\Tool\GameDataMaker\20241121_180832\main.exe conv-param %RES_PATH%\TestAssets\Paramater %RES_PATH%\Orginal\Paramater\Test.xlsm

REM ゲームで使うフォントを作成
%SCRIPT_PATH%\Tool\FontConverter\20241214_173447\main.exe conv %RES_PATH%\Orginal\Font\FontConverterConfig.ini %RES_PATH%\TestAssets\Font %RES_PATH%\Orginal\Font\PixelMplus10-Bold.ttf TestFont.ttf
%SCRIPT_PATH%\Tool\FontConverter\20241214_173447\main.exe conv %RES_PATH%\Orginal\Font\FontConverterConfig.ini %RES_PATH%\Assets\Font %RES_PATH%\Orginal\Font\PixelMplus10-Bold.ttf Font.ttf

REM アセットファイルを読み込みディレクトリに転送
call %SCRIPT_PATH%\HobbyEngine\Script\MoveDir.bat %SCRIPT_PATH%\Game\Resource\Assets\ %SCRIPT_PATH%\Bin\SuperStar\Debug\Assets
if %errorlevel% neq 0 pause & exit /b %errorlevel%

call %SCRIPT_PATH%\HobbyEngine\Script\MoveDir.bat %SCRIPT_PATH%\Game\Resource\Assets\ %SCRIPT_PATH%\Bin\SuperStar\Release\Assets
if %errorlevel% neq 0 pause & exit /b %errorlevel%

REM call %SCRIPT_PATH%\HobbyEngine\Script\MoveDir.bat %SCRIPT_PATH%\Game\Resource\TestAssets\ %SCRIPT_PATH%\Bin\Test\Debug\Assets
REM if %errorlevel% neq 0 pause & exit /b %errorlevel%
call %SCRIPT_PATH%\HobbyEngine\Script\MoveDir.bat %SCRIPT_PATH%\Test\Resource\Assets\ %SCRIPT_PATH%\Bin\Test\Debug\Assets
if %errorlevel% neq 0 pause & exit /b %errorlevel%

endlocal

REM 正常終了
echo "All Complete" & pause & exit /b 0
