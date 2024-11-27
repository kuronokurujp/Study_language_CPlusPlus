@echo off

rem アセットデータを更新バッチ

setlocal

rem パラメータデータをコンバート
Tool\GameDataMaker\20241121_180832\main.exe conv-param .\Game\Resource\Assets\Paramater .\Game\Resource\Orginal\Paramater\Game.xlsm

rem アセットファイルを読み込みディレクトリに転送
call .\HobbyEngine\Script\MoveDir.bat .\Game\Resource\Assets\ .\Bin\SuperStar\Debug\Assets
call .\HobbyEngine\Script\MoveDir.bat .\Game\Resource\Assets\ .\Bin\SuperStar\Release\Assets

call .\HobbyEngine\Script\MoveDir.bat .\Game\Resource\Assets\ .\Bin\Test\Debug\Assets
pause & exit /b %errorlevel%

endlocal
