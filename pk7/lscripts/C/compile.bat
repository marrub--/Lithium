@echo off
set PATH=%PATH%;C:\GDCC
C:\msys64\usr\bin\make.exe -j4
if errorlevel 1 (
   pause
)
