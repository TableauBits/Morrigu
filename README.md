
# Morrigu
![Morrigu_logo_temp](resources/Logo_TEMP.jpg)

# How to install: 
Morrigu uses CMake as build tool and Conan as packet manager. This makes the setup process relatively easy.  
You need [CMake 3.15+](https://cmake.org/download/), and a somewhat recent version of [Conan](https://conan.io/downloads.html) (tested on 1.26.1) with the [bincrafters remote](https://docs.conan.io/en/latest/uploading_packages/remotes.html#bincrafters). Morrigu uses the `cmake_multi` conan generator, so you should be able to install both Debug and Release versions of the dependencies and use both without having to switch context. To make an "out of source build", the steps are then simply:
```bash
mkdir build
cd build
conan install .. -s build_type=Debug
conan install .. -s build_type=Release
cmake -G "<GENERATOR>" ..
```
Where `<GENERATOR>` is a [CMake generator](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html) supported by the version you are using. Not providing a `G` flag should fall back to a default value (defined by your cmake installation).

To actually build, you can use the tool that cmake generated for, or use the universal build interface provided by cmake :
```bash
cmake --build . --config <CONFIG>
```
Where `<CONFIG>` is either `Debug` or `Release`. Just make sure the right dependencies are installed with conan (see above).

Note: The `conan install` lines of the script might need the `--build=<SOURCE_CODE_LIBS>` option to build necessary packages from source as debug or release (this might be necessary if you have the "missing PDB" warning and want to make it go away, but it should not prevent you from compiling successfully). It is also important to note that you don't have to install both versions of the dependencies. If you're only intersted in the release, you can skip the first `conan install` command. 

If you are building spdlog and/or fmt make sure to setup your conan for a gcc version newer than 5.0. From [conan's getting started](https://docs.conan.io/en/latest/getting_started.html):
```bash
conan profile new default --detect  # Generates default profile detecting GCC and sets old ABI
conan profile update settings.compiler.libcxx=libstdc++11 default  # Sets libcxx to C++11 ABI
```