@echo OFF
call build_update.bat
pushd ..\app
echo.
echo ****************************************
echo ********        Native        **********
echo ****************************************
echo.
call %NDK_ROOT%\ndk-build
REM call Y:\android-ndk-r14\ndk-build
popd