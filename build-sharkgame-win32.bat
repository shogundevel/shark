
tcc "%~dp0\cshark\sharkgame.c" -DSDL_MAIN_HANDLED "-I%~dp0\cshark" "-I%~dp0\include\SDL2" "-L%~dp0\bin" -lSDL2 -lSDL2_ttf -lSDL2_image -llibSDL2_gfx-1-0-0 -o "%~dp0\bin\sharkgame.exe"
tcc "%~dp0\cshark\sharkgame.c" -DSDL_MAIN_HANDLED "-I%~dp0\cshark" "-I%~dp0\include\SDL2" "-L%~dp0\bin" -lSDL2 -lSDL2_ttf -lSDL2_image -llibSDL2_gfx-1-0-0 -o "%~dp0\bin\sharkgamew.exe" -mwindows
