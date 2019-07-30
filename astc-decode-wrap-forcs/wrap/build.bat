rd /S /Q build\win1
md build\win1
cd build\win1

cmake -G "Visual Studio 15 2017 Win64" CMAKE_BUILD_TYPE=Debug ../../

msbuild astc_wrap.sln

pause