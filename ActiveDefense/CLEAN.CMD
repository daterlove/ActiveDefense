del /f /q /s *.sdf
del /f /q /s /A h *.suo
del /f /q /s ActiveDefense\*.aps
rd /s /q ipch
rd /s /q Release
rd /s /q debug
rd /s /q ActiveDefense\debug
rd /s /q ActiveDefense\Release
pause