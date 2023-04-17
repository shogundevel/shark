
mkdir temp

bin\sharkg.exe shk sharkg.shg temp\sharkg.shk include
bin\sharkg.exe shk sharkglink.shg temp\sharkglink.shk include

bin\sharkglink.exe shk temp\sharkg.shk sharkg.shk lib\sharkgsystem.shk
bin\sharkglink.exe shk temp\sharkglink.shk sharkglink.shk lib\sharkgsystem.shk

bin\shark.exe tool build sharkgtool.shk temp\g

if not errorlevel 1 (
    copy /Y temp\* /B bin
)

rmdir /S /Q temp
del sharkg.shk
del sharkglink.shk
