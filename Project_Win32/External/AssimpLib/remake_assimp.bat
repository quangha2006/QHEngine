@echo off
REM pushd .\assimp
if exist "CMakeCache.txt" del /Q "CMakeCache.txt"
call cmake -G "Visual Studio 14 2015"
REM popd
pause