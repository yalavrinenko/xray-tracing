#C:\cygwin64\home\Overlord\tmp\Ray-tracing\sys\Python27\App\Scripts\pyi-makespec.exe --onedir ..\Base.py -n Ray-tracing
#@pause
C:\cygwin64\home\Overlord\tmp\Ray-tracing\sys\Python27\App\Scripts\pyinstaller.exe --onedir Ray-tracing.spec

mkdir dist\Ray-tracing\sys
mkdir dist\Ray-tracing\par
mkdir dist\Ray-tracing\results

copy ..\sys\spectr.glade dist\Ray-tracing\sys
copy ..\sys\pattern.par dist\Ray-tracing\sys
copy ..\sys\trans-db.txt dist\Ray-tracing\sys
xcopy ..\example dist\Ray-tracing\example
@pause