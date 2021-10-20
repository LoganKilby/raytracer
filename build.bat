@echo off

ctime -begin timing.ctm

set opts=-FC -GR- -EHa- -nologo -O2 -Zo -Oi -fp:fast -fp:except- -Gm- -GR- -EHa- -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -wd4127 -FC -Z7 -DRAYTRACER_SLOW /IC:\Library\glm\glm /IC:\Library\glm\glm\gtc /IC:\Library\glm\glm\ext

set compiler_flags=-DCOMPILER_MSVC -D_CRT_SECURE_NO_WARNINGS %opts%

REM /IC:\Library\glm\glm\gtx
set code=%cd%
pushd build
cl %compiler_flags% %code%\main.cpp -Feraytracer kernel32.lib
popd

ctime -end timing.ctm %last_error%
