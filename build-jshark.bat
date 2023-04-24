
mkdir temp
javac -d temp -sourcepath jshark jshark/shark/Shark.java
cd temp
jar cfe ../bin/jshark.jar shark.Shark *
cd ../
rmdir /S /Q temp

mkdir temp
javac -d temp -sourcepath jshark jshark/GameShark.java
cd temp
jar cfe ../bin/jgameshark.jar GameShark *
cd ../
rmdir /S /Q temp
