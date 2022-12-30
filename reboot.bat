
mkdir temp
bin\shark.exe tool build include\shark.shk temp\gameshark.shar
bin\shark.exe tool build include\system.shk temp\system.shar
bin\shark.exe tool build sharktool.shk temp\tool
if not errorlevel 1 (
    copy /Y temp\* /B bin
)
rmdir /S /Q temp
mkdir bin\rt
copy /Y rt\* /B bin\rt
