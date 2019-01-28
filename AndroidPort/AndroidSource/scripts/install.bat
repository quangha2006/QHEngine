@echo off


set BUILD_TYPE=%1
set INSTALL_TYPE=installDebug

if %BUILD_TYPE%==Release (
  set INSTALL_TYPE=installRelease
) else if %BUILD_TYPE%==Debug (
  set INSTALL_TYPE=installDebug
) else (
  echo Error : Invalid %BUILD_TYPE%.
  exit 1
)
pushd ..
echo.
echo *************************************
echo *****     Install apk %BUILD_TYPE%  *****
echo *************************************
echo.
    call gradlew.bat %INSTALL_TYPE%
popd