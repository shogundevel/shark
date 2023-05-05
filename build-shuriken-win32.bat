
tcc "%~dp0\cshark\shuriken.c" -DSHARK_UNBOX -DSDL_MAIN_HANDLED "-I%~dp0\cshark" "-I%~dp0\include\SDL2" "-L%~dp0\bin" -lSDL2 -lSDL2_ttf -o "%~dp0\bin\shuriken.exe"
tcc "%~dp0\cshark\shuriken.c" -DSHARK_UNBOX -DSDL_MAIN_HANDLED "-I%~dp0\cshark" "-I%~dp0\include\SDL2" "-L%~dp0\bin" -lSDL2 -lSDL2_ttf -o "%~dp0\bin\shurikenw.exe" -mwindows
"%~dp0\shark" tool build "%~dp0\include\default\shuriken.shk" "%~dp0\bin\shuriken.shar"
