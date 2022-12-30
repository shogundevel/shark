
gcc cshark/gameshark.c -o bin/gameshark.exe -DSHARK_UNBOX `pkg-config --cflags --libs-only-l --libs-only-L SDL2 SDL2_mixer SDL2_image SDL2_ttf SDL2_net SDL2_gfx`
gcc cshark/gameshark.c -o bin/gamesharkw.exe -DSHARK_UNBOX `pkg-config --cflags --libs SDL2 SDL2_mixer SDL2_image SDL2_ttf SDL2_net SDL2_gfx`
