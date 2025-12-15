@echo off
setlocal
if not exist build goto :nobuild
if exist dist rmdir /s /q dist
mkdir dist
xcopy /e /i build\\Release dist\\bin
xcopy /e /i data dist\\data
:nobuild
