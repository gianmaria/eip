@echo off

if not exist "build/" (
    mkdir build
)

pushd build

cl ..\eip.cpp ^
/std:c++latest ^
/D "NDEBUG" /D "_CONSOLE" /D "_UNICODE" /D "UNICODE" ^
/Wall ^
/wd4668 /wd4820 /wd5039 /wd5045 /wd4711 /wd4710 ^
/Zi /O2 /MT ^
/EHsc ^
/Gm- /GL ^
/external:anglebrackets /analyze:external- /external:W0 ^
/utf-8 ^
/link /SUBSYSTEM:CONSOLE /DEBUG 

popd
