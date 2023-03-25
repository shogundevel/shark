
gcc cshark/gameshark.c -o bin/gameshark -DSHARK_UNBOX -lm `pkg-config --cflags --libs sdl2 SDL2_image SDL2_ttf SDL2_gfx`

