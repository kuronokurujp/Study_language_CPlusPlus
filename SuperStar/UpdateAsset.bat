@echo off

rem アセットデータを更新バッチ

setlocal

call .\HobbyEngine\Script\MoveDir.bat .\Game\Resource\Assets\ .\Bin\SuperStar\Debug\Assets
call .\HobbyEngine\Script\MoveDir.bat .\Game\Resource\Assets\ .\Bin\SuperStar\Release\Assets

call .\HobbyEngine\Script\MoveDir.bat .\Game\Resource\Assets\ .\Bin\Test\Debug\Assets
pause & exit /b %errorlevel%

endlocal
