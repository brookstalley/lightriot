C:\\MinGW\\msys\\1.0\\bin\\mintty.exe /bin/bash -l -c "cd /c/Users/brook/Source/lightriot && make BOARD=samr21-xpro || read -p 'Ctrl-c to abort flashing'"
..\RIOT\dist\tools\edbg\edbg --serial ATML2127031800010160 -t atmel_cm0p -b -e -v -p -f bin\samr21-xpro\lightriot.bin
..\RIOT\dist\tools\edbg\edbg --serial ATML2127031800010131 -t atmel_cm0p -b -e -v -p -f bin\samr21-xpro\lightriot.bin
