
gcc cshark/shuriken.c -o bin/shuriken -DSHARK_UNBOX -lm `pkg-config --cflags --libs sdl2 SDL2_ttf`
shark tool build include/default/shuriken.shk bin/shuriken.shar
