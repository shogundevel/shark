
mkdir temp

java -jar bin/sharkg.jar _ c sharkg.shg temp/sharkg.c
if errorlevel 0 (

java -jar bin/sharkg.jar _ c sharkglink.shg temp/sharkglink.c
if errorlevel 0 (

tcc temp/sharkg.c lib/shark.c lib/shark_system.c lib/shark_main.c -Ilib -w -o bin/sharkg.exe
tcc temp/sharkglink.c lib/shark.c lib/shark_system.c lib/shark_main.c -Ilib -w -o bin/sharkglink.exe

del temp\sharkg.c
del temp\sharkglink.c
rmdir temp

))
