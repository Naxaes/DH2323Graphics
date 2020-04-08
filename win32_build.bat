@echo off

REM Compiler switches: https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-alphabetically?view=vs-2019

REM WX     - Treat warnings as errors.
REM wdxxx  - Disable warning xxxx.
REM Zi     - Generate debug information.
REM Oi     - Use intrinsics.
REM EHsc   - 
REM EHa    - 
REM nologo - Don't show compiler info when compiling.
REM subsystem:windows,5.1 - Make compatible with Windows XP.
REM MD     - Dynamically link multi-threaded CRT.
REM MT     - Statically link multi-threaded CRT. Required to make compatiable with other Windows versions.
REM opt:ref - Try to remove unused code.
REM GR-    - Disable run-time type information, RTTI.
REM Fm     - Creates a map file at specified path.
REM LD     - Creates a dynamic-link library.
REM LDd    - Creates a debug dynamic-link library.
REM incremental:no - Turns off incremental builds.
REM Od     - Disables optimization.

REM Removed -incremental:no -opt:ref 

REM ---- WARNINGS ----
REM C4244 - 
REM C4201 - Non-standard extension used.
REM C4100 - Unreferenced formal parameter.
REM C4189 - Local variable is initialized but not referenced.
REM C4505 - Unreferenced local function has been removed.

set DEBUG=1
set GAME=1
set PLATFORM=1

if "%1"=="release"  set DEBUG=0
if "%2"=="release"  set DEBUG=0
 
if "%1"=="platform" set GAME=0
if "%2"=="platform" set GAME=0

if "%1"=="game" set PLATFORM=0
if "%2"=="game" set PLATFORM=0

REM Visual Studio locks the PDB, even though we release the DLL.
REM Workaround is to generate a new PDB file everytime and delete the previous when
REM Visual Studio realeses them.
set PDB_OUTPUT="temp_%date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%.pdb"

set WARNINGS=-W4 -WX -wd4100 -wd4201 -wd4505

set SHARED_COMPILER_FLAGS=-Oi -MT -fp:fast -nologo -GR- -EHa- -D_CRT_SECURE_NO_WARNINGS %WARNINGS%
set SHARED_LINKER_FLAGS=-incremental:no -opt:ref

set WIN32_COMPILER_FLAGS=
set WIN32_LINKER_FLAGS=user32.lib gdi32.lib winmm.lib Xaudio2.lib
set WIN32_OUTPUT_FILE=
set WIN32_SOURCE_FILES=..\source\win32_main.cpp
set WIN32_INCLUDE_FOLDER=

set GAME_COMPILER_FLAGS=-LDd
set GAME_LINKER_FLAGS=-DLL -PDB:%PDB_OUTPUT%
set GAME_OUTPUT_FILE=main.dll
set GAME_SOURCE_FILES=..\source\lab1.cpp
set GAME_INCLUDE_FOLDER=..\libraries\glm


if "%DEBUG%"=="1" (
	echo Compiling with debug configurations.
	set SHARED_COMPILER_FLAGS=-Od -Zi %SHARED_COMPILER_FLAGS%
	set GAME_COMPILER_FLAGS=-LDd %GAME_COMPILER_FLAGS%
) else (
	echo Compiling with release configurations.
	set SHARED_COMPILER_FLAGS=-O2 %SHARED_COMPILER_FLAGS%
	set GAME_COMPILER_FLAGS=-LD %GAME_COMPILER_FLAGS%
)


if not exist "build"  mkdir "build"
pushd build

del *.pdb > nul 2>&1

if "%GAME%"=="1" (
	cl  %SHARED_COMPILER_FLAGS% %GAME_COMPILER_FLAGS%    %GAME_SOURCE_FILES%  -I%GAME_INCLUDE_FOLDER%  -Fe:%GAME_OUTPUT_FILE%   -link  %SHARED_LINKER_FLAGS% %GAME_LINKER_FLAGS%
) 
if "%PLATFORM%"=="1" (
	cl  %SHARED_COMPILER_FLAGS% %WIN32_COMPILER_FLAGS%   %WIN32_SOURCE_FILES% -I%WIN32_INCLUDE_FOLDER%	-opt:ref				-link  %SHARED_LINKER_FLAGS% %WIN32_LINKER_FLAGS% 
)


popd