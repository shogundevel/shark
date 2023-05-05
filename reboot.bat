
mkdir temp
bin\shark.exe tool build include\default\shark.shk temp\gameshark.shar
bin\shark.exe tool build include\default\system.shk temp\system.shar
bin\shark.exe tool build sharktool.shk temp\tool
if not errorlevel 1 (
    copy /Y temp\* /B bin
)
rmdir /S /Q temp

mkdir bin\rt
copy /Y rt\* /B bin\rt

mkdir bin\lib
copy /Y lib\* /B bin\lib
