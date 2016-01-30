
@set PROJ_PATH=\path\to\project
@set GCC_PATH=\path\to\gccs\dir
@set GCC=%GCC_PATH%\gcc
@set CFLAGS=-I\path\to\includes -m32 -DALIGN=4

@set LFLAGS=-mwindows -lmingw32 -lGFraMe
@set LFLAGS=%LFLAGS% -L\path\to\libs
@set LFLAGS=%LFLAGS% -lSDL2main

mkdir obj\win\debug obj\win\release bin\win

:: Compile release stuff
%GCC% %CFLAGS% -c %PROJ_PATH%\src\assets.c %PROJ_PATH%\obj\debug\assets.o
%GCC% %CFLAGS% -c %PROJ_PATH%\src\collision.c %PROJ_PATH%\obj\debug\collision.o
%GCC% %CFLAGS% -c %PROJ_PATH%\src\config.c %PROJ_PATH%\obj\debug\config.o
%GCC% %CFLAGS% -c %PROJ_PATH%\src\gamestate.c %PROJ_PATH%\obj\debug\gamestate.o
%GCC% %CFLAGS% -c %PROJ_PATH%\src\global.c %PROJ_PATH%\obj\debug\global.o
%GCC% %CFLAGS% -c %PROJ_PATH%\src\input.c %PROJ_PATH%\obj\debug\input.o

:: Compile the release binary
%GCC% %CFLAGS% -o %PROJ_PATH%\bin\win\game.exe %PROJ_PATH%\obj\release\* %LFLAGS%

:: Add debug parameters
@set CFLAGS=%CFLAGS% -g -O0 -DDEBUG

:: Compile debug stuff
%GCC% %CFLAGS% -c %PROJ_PATH%\src\assets.c %PROJ_PATH%\obj\release\assets.o
%GCC% %CFLAGS% -c %PROJ_PATH%\src\collision.c %PROJ_PATH%\obj\release\collision.o
%GCC% %CFLAGS% -c %PROJ_PATH%\src\config.c %PROJ_PATH%\obj\release\config.o
%GCC% %CFLAGS% -c %PROJ_PATH%\src\gamestate.c %PROJ_PATH%\obj\release\gamestate.o
%GCC% %CFLAGS% -c %PROJ_PATH%\src\global.c %PROJ_PATH%\obj\release\global.o
%GCC% %CFLAGS% -c %PROJ_PATH%\src\input.c %PROJ_PATH%\obj\release\input.o

:: Compile the debug binary
%GCC% %CFLAGS% -o %PROJ_PATH%\bin\win\game_debug.exe %PROJ_PATH%\obj\debug\* %LFLAGS%

