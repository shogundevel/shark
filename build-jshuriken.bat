
mkdir temp
javac -d temp -sourcepath jshark jshark/Shuriken.java
cd temp
jar cfe ../bin/jshuriken.jar Shuriken *
cd ../
rmdir /S /Q temp
