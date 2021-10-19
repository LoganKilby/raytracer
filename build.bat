@echo off

ctime -begin timing.ctm

set opts=-FC -GR- -EHa- -nologo -O2 -Zi -DRAYTRACER_SLOW /IC:\Library\glm\glm /IC:\Library\glm\glm\gtc /IC:\Library\glm\glm\ext

REM /IC:\Library\glm\glm\gtx
set code=%cd%
pushd build
cl %opts% %code%\main.cpp -Feraytracer kernel32.lib
popd

ctime -end timing.ctm %last_error%
