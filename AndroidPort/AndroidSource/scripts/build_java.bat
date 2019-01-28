@echo OFF
REM pushd ..\app
REM call ant debug
REM using gradle build
REM call build_update.bat
set arg=%1
set BUILD_TYPE=assembleRelease
if %arg%==Release (
    set BUILD_TYPE=assembleRelease
) else if %arg%==Debug (
    set BUILD_TYPE=assembleDebug
) else (
    echo Error : Invalid %arg%.
    exit 1
)
pushd ..
echo.
echo ****************************************
echo ********     Java %arg%    **********
echo ****************************************
echo.
if exist bin\*.apk (
del /Q .\bin
)
call gradlew.bat %BUILD_TYPE%
popd