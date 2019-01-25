@echo OFF
REM pushd ..\app
REM call ant debug
REM using gradle build
REM call build_update.bat
pushd ..
echo.
echo ****************************************
echo ********         Java         **********
echo ****************************************
echo.
if exist bin\*.apk (
del /Q .\bin
)
call gradlew.bat assembleRelease
if exist app\build\outputs\apk\release\*.apk (
if not exist .\bin (
mkdir .\bin\
)
copy app\build\outputs\apk\release\*.apk .\bin\
)
popd