
gcc cshark/sharkgame.c -o bin/sharkgame -DSHARK_UNBOX -lm `pkg-config --cflags --libs sdl2 SDL2_image SDL2_ttf SDL2_gfx`
