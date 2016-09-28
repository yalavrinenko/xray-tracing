set dirname_64=win-x86_64
set dirname_32=win-x86

::set PATH=%PATH%;C:\Mingw-w64\x86_64-5.3.0-posix-seh-rt_v4-rev0\mingw64\bin

set TMPPATH=%PATH%

mkdir %dirname_64%
mkdir %dirname_32%

set PATH=%TMPPATH%;C:\MinGW\bin
for %%x in (*.cpp) do gcc.exe -m32  -D__USE_MINGW_ANSI_STDIO -D WIN32 -D NO_OMP_SUPP -D M_PI=3.14159265358979 -fPIC -fopenmp -c -std=c++11 -w %%x -o %dirname_32%/%%x.o
g++.exe -shared -m32 -Wl,-Bstatic -o ./%dirname_32%/libRaytrace.dll ./%dirname_32%/*.o
@pause

set PATH=%TMPPATH%;C:\mingw-w64\x86_64-6.2.0-posix-seh-rt_v5-rev1\mingw64\bin
for %%x in (*.cpp) do gcc.exe -m64 -D__USE_MINGW_ANSI_STDIO -D WIN32 -D M_PI=3.14159265358979 -fPIC -fopenmp -c -std=c++11 -w %%x -o %dirname_64%/%%x.o
g++.exe -shared -fopenmp -m64 -Wl,-Bstatic -o ./%dirname_64%/libRaytrace.dll ./%dirname_64%/*.o

@pause

