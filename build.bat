CD /d %~dp0
del /f /q ..\plugin\plugorg.asi
cl -c /GS- /GF /Ox /MD /EHsc plugorg.cpp
link /dll /entry:_DllMainCRTStartup@12 plugorg.obj /out:..\plugin\plugorg.asi
CD /d %~dp0
del /f /q  "*.obj"