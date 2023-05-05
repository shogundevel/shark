
mkdir temp
mkdir classes

call sharkg java sharkg/shark_main.shg temp/out.java
call sharkglink java temp/out.java temp/Main.java lib/sharksystem.java
javac -d classes temp/Main.java

cd classes
jar cfe ../bin/sharkg.jar Main *
cd ..

rmdir /S /Q classes
mkdir classes

call sharkg java sharkg/shark_link.shg temp/out.java
call sharkglink java temp/out.java temp/Main.java lib/sharksystem.java
javac -d classes temp/Main.java

cd classes
jar cfe ../bin/sharkglink.jar Main *
cd ..

rmdir /S /Q temp
rmdir /S /Q classes
