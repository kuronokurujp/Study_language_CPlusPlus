@echo off

REM 構築した開発環境を破棄

setlocal

REM エンジンの環境構築を破棄
set SCRIPT_PATH=%~dp0HobbyEngine\Script\Build\

call "%SCRIPT_PATH%EnvClean.bat" %~dp0
if %errorlevel% neq 0 pause & exit /b %errorlevel%

REM 正常終了
echo "all complete" & pause & exit /b 0

endlocal
