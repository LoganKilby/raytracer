@echo off

set opts=-FC -GR- -EHa- -nologo -Zi -DRAYTRACER_SLOW
set code=%cd%
pushd build
cl %opts% %code%\main.cpp -Feraytracer
popd
