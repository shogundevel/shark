
gcc cshark/sharkgame.c -o bin/sharkgame.exe -O3 `pkg-config --cflags --libs-only-l --libs-only-L SDL2 SDL2_mixer SDL2_image SDL2_ttf SDL2_net SDL2_gfx`
gcc cshark/sharkgame.c -o bin/sharkgamew.exe -O3 `pkg-config --cflags --libs SDL2 SDL2_mixer SDL2_image SDL2_ttf SDL2_net SDL2_gfx`
