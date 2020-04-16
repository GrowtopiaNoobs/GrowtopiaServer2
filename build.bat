git submodule update --init --recursive
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
msbuild ALL_BUILD.vcxproj /p:configuration=release
