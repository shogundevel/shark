
mkdir temp
javac -d temp -sourcepath jshark jshark/shark/Shark.java
jar cfe bin/jshark.jar shark.Shark -C temp *
rmdir /S /Q temp

mkdir temp
javac -d temp -sourcepath jshark jshark/GameShark.java
jar cfe bin/jgameshark.jar GameShark -C temp *
rmdir /S /Q temp
