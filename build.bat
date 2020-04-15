git submodule update --init --recursive
mkdir build
cd build
cmake ..
msbuild ALL_BUILD.vcxproj
