
:boot

compile c include/system.shk temp/out.bin
link c temp/out.bin null temp/system.shar

compile c include/shark.shk temp/out.bin
link c temp/out.bin null temp/gameshark.shar

compile c sharktool.shk temp/out.bin
link c temp/out.bin sharktool temp/tool

unlink temp/out.bin
