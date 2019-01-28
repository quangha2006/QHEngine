REM call build_native.bat
setlocal EnableExtensions EnableDelayedExpansion
set arg=%1
call external_native_fastbuild.bat %arg%
if NOT "!errorlevel!"=="0" (
    echo COMPILE ERROR code returned by fbuild.exe: !errorlevel!
    pause
    exit !errorlevel!
)
call build_java.bat %arg%
if NOT "!errorlevel!"=="0" (
    echo COMPILE ERROR code returned by java: !errorlevel!
    pause
    exit !errorlevel!
)
call install.bat %arg%