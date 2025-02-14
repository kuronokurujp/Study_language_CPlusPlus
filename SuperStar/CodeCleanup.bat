@echo off
chcp 65001

REM cpp/h/hppファイルのコード整理を一括で行うバッチファイル

setlocal

REM エンジンにあるコード整理を行うスクリプトを起動
call .\HobbyEngine\Script\CodeCleanup\CodeCleanup.bat .\HobbyEngine
call .\HobbyEngine\Script\CodeCleanup\CodeCleanup.bat .\HobbyPlugin
call .\HobbyEngine\Script\CodeCleanup\CodeCleanup.bat .\Game

endlocal

REM 正常終了
echo "all complete" & pause & exit /b 0