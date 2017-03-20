pyinstaller.exe --onedir --distpath ..\ Ray-tracing.spec

mkdir ..\Ray-tracing\sys
mkdir ..\Ray-tracing\par
mkdir ..\Ray-tracing\results

echo ".par" > ..\Ray-tracing\par\.par
echo ".results" > ..\Ray-tracing\results\.results

copy ..\sys\spectr.glade ..\Ray-tracing\sys
copy ..\sys\pattern.par ..\Ray-tracing\sys
copy ..\sys\trans-db.txt ..\Ray-tracing\sys
xcopy ..\Input ..\Ray-tracing\Input