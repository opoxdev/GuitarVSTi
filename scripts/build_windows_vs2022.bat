@echo off
setlocal enabledelayedexpansion
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 || goto :error
cmake --build build --config Release || goto :error
exit /b 0
:error
exit /b 1
