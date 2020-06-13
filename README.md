
# Morrigu
![Morrigu_logo_temp](resources/Logo_TEMP.jpg)

# How to install: 
Morrigu uses CMake as build tool and Conan as packet manager. This makes the setup process relatively easy.  
You need [CMake 3.15+](https://cmake.org/download/), and a somewhat recent version of [Conan](https://conan.io/downloads.html).
To make an "out of source build", the steps are then simply:
```bash
mkdir build
cd build
conan install ..
cmake -G "<GENERATOR>" ..
```
Where `<GENERATOR>` is a [CMake generator](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html) supported by the version you are using.