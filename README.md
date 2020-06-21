
# Morrigu
![Morrigu_logo_temp](resources/Logo_TEMP.jpg)

# How to install: 
Morrigu uses CMake as build tool and Conan as packet manager. This makes the setup process relatively easy.  
You need [CMake 3.15+](https://cmake.org/download/), and a somewhat recent version of [Conan](https://conan.io/downloads.html). To make an "out of source build", the steps are then simply:
```bash
mkdir build
cd build
conan install ..
cmake -G "<GENERATOR>" ..
```
Where `<GENERATOR>` is a [CMake generator](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html) supported by the version you are using.

Note: The penultimate line of the script might need the `-s build_type=[Debug|Release]` and `--build<SOURCE CODE LIBS>` option to build necessary packages from source as debug or release.  
If you are building spdlog and/or fmt make sure to setup your conan for a gcc version newer than 5.0. From [conan's getting started](https://docs.conan.io/en/latest/getting_started.html):
```bash
conan profile new default --detect  # Generates default profile detecting GCC and sets old ABI
conan profile update settings.compiler.libcxx=libstdc++11 default  # Sets libcxx to C++11 ABI
```