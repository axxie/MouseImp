@echo off
set datetime=%date%__%time%
set datetime=%datetime: =%
set datetime=%datetime::=_%
set datetime=%datetime:,=_%
set OutName=Mimp_Release_Win32_%datetime%.log
set OutName64=Mimp_Release_x64_%datetime%.log

@echo on
"%VS80COMNTOOLS%\..\IDE\devenv" MImp.sln /rebuild "Release|Win32" /Out %OutName% >nul 2>nul
@if errorlevel 1 goto prep_build_error
"%VS80COMNTOOLS%\..\IDE\devenv" MImp.sln /rebuild "Release|x64" /Out %OutName64% >nul 2>nul
@if errorlevel 1 goto prep_build_error64

@echo Incrementing build number
cscript IncrementBuildNumber.vbs
@if errorlevel 1 goto error

"%VS80COMNTOOLS%\..\IDE\devenv" MImp.sln /rebuild "Release|Win32" /Out %OutName% >nul 2>nul
@if errorlevel 1 goto error
"%VS80COMNTOOLS%\..\IDE\devenv" MImp.sln /rebuild "Release|x64" /Out %OutName64% >nul 2>nul
@if errorlevel 1 goto error64

@echo BUILD SUCCEEDED

@goto exit

:prep_build_error64
@echo x64 preparation build failed, skiping version increment
:error64
@echo X64 BUILD FAILED, SEE %OutName64%
@goto exit

:prep_build_error
@echo Preparation build failed, skiping version increment
:error
@echo BUILD FAILED, SEE %OutName%

:exit