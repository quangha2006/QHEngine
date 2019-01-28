REM call build_native.bat
setlocal EnableExtensions EnableDelayedExpansion
set arg=%1
call external_native_fastbuild.bat %arg%
if NOT "!errorlevel!"=="0" (
    goto:eof
    REM exit /b !errorlevel!
)
call build_java.bat %arg%
if NOT "!errorlevel!"=="0" (
    goto:eof
    REM exit /b !errorlevel!
)
call install.bat %arg%
:eof