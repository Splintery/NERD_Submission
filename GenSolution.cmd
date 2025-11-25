:: -------------------------------------------------------------------------------
:: Project: Recruitement
:: File: GenSolution.cmd
::
:: Copyright (C) 2018-2019 Nintendo, All rights reserved.
::
:: These coded instructions, statements, and computer programs contain proprietary
:: information of Nintendo and/or its licensed developers and are protected by
:: national and international copyright laws. They may not be disclosed to third
:: parties or copied or duplicated in any form, in whole or in part, without the
:: prior written consent of Nintendo.
::
:: The content herein is highly confidential and should be handled accordingly.
:: -------------------------------------------------------------------------------


:: Helper batch script that generates a Visual Studio solution.
:: Two operation modes are available: interactive or automated.
:: To use the automated mode, simply pass as command line arguments the
:: Visual Studio version and platform to target. such as "GenSolution.cmd VS2010 Cafe".



@echo off

setlocal EnableDelayedExpansion

:: Fully qualified path to the folder this batch file is in
set "HERE=%~dp0"

:: Path to the folder containing the root CMakeLists.txt file
set "SOURCEDIR=%HERE%"

:: Folders for generated files
set "BUILD_DIR=%HERE%\build"
set "LIB_DIR=%HERE%\lib"
set "EXE_DIR=%HERE%\bin"
set "DLL_DIR=%HERE%\bin"

:: Init variables
set OPTIONS=
set ASKPAUSE=0
set AUTOMATED=0

:: Just in case we run on a 32-bit Windows
if not exist "%ProgramFiles(x86)%" set "%ProgramFiles(x86)%=%ProgramFiles%"

if exist "%HERE%buildchain\cmake\bin\cmake.exe" (
    set "CMAKE=%HERE%buildchain\cmake\bin\cmake.exe"
) else (
    set CMAKE=cmake
)
:: ============================================================================
:: Parse arguments

if "%~1"=="--help" (
	echo Usage:
	echo   %~n0 version platform [--build:config]
	echo.
	echo Where:
	echo   version     The Visual Studio: VS2015, VS2017 or VS2019.
	echo   platform    The platform to target: Win32, Win64.
	echo   config      The configuration to build ^(typically, Debug or Release^).
	exit /b 0
)

if not "%~1"=="" (
	if not "%~2"=="" (
		set AUTOMATED=1
		set VERSION=%~1
		set PLATFORM=%~2
		shift
		shift
	) else (
		echo Two arguments expected, got only one >&2
		exit /b 1
	)
)

set BUILD_CONFIG=
:ParseOptions
set ARG=%~1
set ARG8=%ARG:~0,8%
if "%ARG8%"=="--build:" (
	set BUILD_CONFIG=%ARG:--build:=%
	shift
) else if "%ARG%"=="--cpp11" (
	set CORE_ENABLE_CPP11=1
	shift
) else (
	goto DoneParsingOptions
)
goto ParseOptions
:DoneParsingOptions


:: ============================================================================
:: Choose VS version

:AskVersion
if not %AUTOMATED%==0 goto %VERSION%
echo Which version of Visual Studio do you want to generate a solution for?
echo  1 - Visual Studio 14 (2015)
echo  2 - Visual Studio 15 (2017)
echo  3 - Visual Studio 16 (2019)
echo  4 - Visual Studio 17 (2022)


set /p VERSION=
if "%VERSION%"=="1" goto VS2015
if "%VERSION%"=="2" goto VS2017
if "%VERSION%"=="3" goto VS2019
if "%VERSION%"=="4" goto VS2022
echo.
goto AskVersion

:VS2015
:VC14
set GENERATOR=Visual Studio 14
set VS_YEAR=2015
goto ChoosePlatform

:VS2017
:VC15
set GENERATOR=Visual Studio 15
set VS_YEAR=2017
goto ChoosePlatform

:VS2019
:VC16
set GENERATOR=Visual Studio 16 2019
set VS_YEAR=2019
goto ChoosePlatform

:VS2022
:VC17
set GENERATOR=Visual Studio 17 2022
set VS_YEAR=2022
goto ChoosePlatform

:: ============================================================================
:: Choose Platform

:ChoosePlatform
if not %AUTOMATED%==0 goto %PLATFORM%
set "PLATFORM_LIST=WIN32 WIN64"
for %%p in (%PLATFORM_LIST%) do (
	set %%p_NUM=0
)
echo.
set WIN32_NUM=1
set WIN64_NUM=2
set NUM=3
)
echo Which platform to you want to build for?
if %WIN32_NUM%       neq 0 echo  %WIN32_NUM% - Win32
if %WIN64_NUM%       neq 0 echo  %WIN64_NUM% - Win64
set /p PLATFORM=
for %%p in (%PLATFORM_LIST%) do (
	if %PLATFORM% equ !%%p_NUM! (
		goto %%p
	)
)
echo.
goto ChoosePlatform

:x86
:Win32
set FOLDER=windows-x86-msvc%VS_YEAR%-vs%VS_YEAR%
set OPTIONS=%1 %2 %3 %4 %5 %6 %5 %8 %9
goto Generate

:x86-64
:amd64
:x64
:Win64
set FOLDER=windows-x64-msvc%VS_YEAR%-vs%VS_YEAR%
set OPTIONS=%1 %2 %3 %4 %5 %6 %5 %8 %9
set CMAKE_FORCE_WIN64=1
set ADD64TOGEN=1
if "%GENERATOR%"=="Visual Studio 16 2019" (
    set GENERATOROPTIONS="-Ax64"
    set ADD64TOGEN=0
)

if "%GENERATOR%"=="Visual Studio 17 2022" (
    set GENERATOROPTIONS="-Ax64"  
    set ADD64TOGEN=0
)

if %ADD64TOGEN% neq 0 (
    set GENERATOR=%GENERATOR% Win64
)


goto Generate

:x86-clang
:Win32-clang
:Win32_Clang
set FOLDER=windows-x86-clang-vs%VS_YEAR%
set "TOOLSET=-T %CLANG_TOOLSET%"
set OPTIONS=%1 %2 %3 %4 %5 %6 %5 %8 %9
goto Generate

:x86-64-clang
:amd64-clang
:x64-clang
:Win64-clang
:Win64_Clang
set FOLDER=windows-x64-clang-vs%VS_YEAR%
set "TOOLSET=-T %CLANG_TOOLSET%"
set OPTIONS=%1 %2 %3 %4 %5 %6 %5 %8 %9
goto Generate


:: ============================================================================
:: Generate solution

:: Check if a solution already exists
:Generate
if not %AUTOMATED%==0               goto CallCMake
if not exist "%BUILD_DIR%\%FOLDER%" goto CallCMake
:AskDelete
echo.
echo A solution already exists, do you want to delete it? (y/n^)
set /p SUPPRESS=
if "%SUPPRESS%"=="y" (rmdir /s /q "%BUILD_DIR%\%FOLDER%" && goto CallCMake)
if "%SUPPRESS%"=="n" goto CallCMake
echo.
goto AskDelete

:: Actually generate the solution (and optionally build it)
:CallCMake
echo.
if not exist "%BUILD_DIR%\%FOLDER%" mkdir "%BUILD_DIR%\%FOLDER%"
pushd "%BUILD_DIR%\%FOLDER%"
%CMAKE% -G "%GENERATOR%" %TOOLSET% "-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=%LIB_DIR%\%FOLDER%" "-DCMAKE_RUNTIME_OUTPUT_DIRECTORY=%EXE_DIR%\%FOLDER%" "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=%DLL_DIR%\%FOLDER%" %OPTIONS% "%SOURCEDIR%" || goto Error
:AskOpen
if %AUTOMATED%==0 (
	echo.
	echo The Solution has been generated in "%BUILD_DIR%\%FOLDER%"
	echo "Do you want to open it (y/n)?"
	set /p OPEN=
	if "!OPEN!"=="n" exit /b 0
	if "!OPEN!"=="y" (
		for %%f in ("%BUILD_DIR%\%FOLDER%\*.sln") do (
			start "" "%%~ff"
			exit /b 0
		)
	)
	goto AskOpen
)
:Build
if not "%BUILD_CONFIG%"=="" (
	%CMAKE% --build . --config %BUILD_CONFIG% --clean-first --use-stderr || goto Error
	shift
	goto Build
)
popd

:End
exit /b 0

:Error
popd
if %AUTOMATED%==0 (
	echo.
	pause
)
exit /b 1
