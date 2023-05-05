
mkdir temp

java -jar bin/sharkg.jar _ c sharkg/shark_main.shg temp/sharkg.c include
java -jar bin/sharkg.jar _ c sharkg/shark_link.shg temp/sharkglink.c include

gcc temp/sharkg.c lib/shark.c lib/shark_system.c lib/shark_main.c -Ilib -lm -w -o bin/sharkg
gcc temp/sharkglink.c lib/shark.c lib/shark_system.c lib/shark_main.c -Ilib -lm -w -o bin/sharkglink

rm temp/sharkg.c
rm temp/sharkglink.c
rmdir temp
