@echo OFF
pushd ..\app
set ext="true";
if not exist "local.properties" set ext=false
if not exist "project.properties" set ext=false
if %ext%==false (
echo.
echo ================= Update Project ======================
echo.
call %ANDROID_SDK_ROOT%\tools\android update project --path . --target android-19
)
popd