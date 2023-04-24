
mkdir temp

java -jar bin/sharkg.jar _ c sharkg.shg temp/sharkg.c
java -jar bin/sharkg.jar _ c sharkglink.shg temp/sharkglink.c

gcc temp/sharkg.c lib/shark.c lib/shark_system.c lib/shark_main.c -Ilib -lm -w -o bin/sharkg
gcc temp/sharkglink.c lib/shark.c lib/shark_system.c lib/shark_main.c -Ilib -lm -w -o bin/sharkglink

rm temp/sharkg.c
rm temp/sharkglink.c
rmdir temp
