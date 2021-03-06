@echo off

set CURDIR=%CD%

cd /d %~dp0

if not defined MSBUILD (
    set MSBUILD="C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe"
)

if not defined TARGET (
    set TARGET=Rebuild
)

if not defined CONFIG (
    set CONFIG=Release
)

if not "%1"=="" (
    set MojoShaderConfig=%1
)else if defined CONFIG (
    set MojoShaderConfig=%CONFIG%
)


set SOLUTION=".\mojoshader\MojoShader.sln"

if not exist %SOLUTION% (
    cd mojoshader
    ..\..\..\External\cmake\bin\cmake.exe -G "Visual Studio 14 2015"
    cd ..
)

%MSBUILD% %SOLUTION% /t:%TARGET% /p:Configuration=%MojoShaderConfig% || goto error

cd /d %CURDIR%

exit /b 0

:error
cd /d %CURDIR%
echo "Error====="
pause
exist /b 1