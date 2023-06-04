
mkdir temp
javac -d temp -sourcepath jshark jshark/shark/Shark.java
cd temp
jar cfe ../bin/jshark.jar shark.Shark *
cd ../
rmdir /S /Q temp

mkdir temp
javac -d temp -sourcepath jshark jshark/SharkGame.java
cd temp
jar cfe ../bin/jsharkgame.jar SharkGame *
cd ../
rmdir /S /Q temp
