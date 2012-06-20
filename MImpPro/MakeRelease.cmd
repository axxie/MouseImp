@echo off
set datetime=%date%__%time%
set datetime=%datetime::=_%
set datetime=%datetime:,=_%
set OutName=Mimp_ReleaseWin32_%datetime%.log

@echo on
"%VS80COMNTOOLS%\..\IDE\devenv" MImp.sln /rebuild "Release|Win32" /Out %OutName% >nul 2>nul
@if errorlevel 1 goto prep_build_error

@echo Incrementing build number
cscript IncrementBuildNumber.vbs
@if errorlevel 1 goto error

"%VS80COMNTOOLS%\..\IDE\devenv" MImp.sln /rebuild "Release|Win32" /Out %OutName% >nul 2>nul
@if errorlevel 1 goto error

@echo BUILD SUCCEEDED

@goto exit

:prep_build_error
echo Preparation build failed, skiping version increment
:error
echo BUILD FAILED, SEE %OutName%

:exit