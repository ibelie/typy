@ECHO OFF

RD /q /s build
MKDIR build
CD build

cmake -G "Visual Studio 14 2015" %~dp0
cmake --build . --config Debug

PAUSE
