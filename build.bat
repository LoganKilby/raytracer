@echo off

ctime -begin timing.ctm

set opts=-FC -GR- -EHa- -nologo -Zi -DRAYTRACER_SLOW /IC:\Library\glm\glm /IC:\Library\glm\glm\gtc 

REM /IC:\Library\glm\glm\gtx
set code=%cd%
pushd build
cl %opts% %code%\main.cpp -Feraytracer
cl %opts% %code%\test.cpp -Fetests
popd

ctime -end timing.ctm %last_error%

pushd build
tests