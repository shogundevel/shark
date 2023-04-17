
mkdir temp
mkdir classes

bin\sharkg java sharkg.shg temp/out.java include
bin\sharkglink java temp/out.java temp/Main.java lib/sharkgsystem.java
javac -d classes temp/Main.java

cd classes
jar cfe ../bin/sharkg.jar Main *
cd ..

rmdir /S /Q classes
mkdir classes

bin\sharkg java sharkglink.shg temp/out.java include
bin\sharkglink java temp/out.java temp/Main.java lib/sharkgsystem.java
javac -d classes temp/Main.java

cd classes
jar cfe ../bin/sharkglink.jar Main *
cd ..

rmdir /S /Q temp
rmdir /S /Q classes
