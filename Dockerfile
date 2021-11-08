FROM ubuntu:20.04

# Installing base dependencies
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update -y
RUN apt-get install -y \
    git \
    cmake \
    wget \
    python3-pip \
    libgl-dev \
    pkg-config \
    sudo \
    software-properties-common

# Installing Vulkan SDK
RUN wget -qO - https://packages.lunarg.com/lunarg-signing-key-pub.asc | apt-key add -
RUN wget -qO /etc/apt/sources.list.d/lunarg-vulkan-focal.list https://packages.lunarg.com/vulkan/lunarg-vulkan-focal.list
RUN apt-get update -y
RUN apt-get install -y \
    vulkan-sdk

# Installing C++20 compiler
RUN add-apt-repository -y \
    ppa:ubuntu-toolchain-r/ppa
RUN apt-get update -y
RUN apt-get install -y gcc-10 g++-10
RUN cp /usr/bin/gcc-10 /usr/bin/cc
RUN cp /usr/bin/g++-10 /usr/bin/c++

# Setting up conan
RUN pip install conan
RUN conan profile new default --detect
RUN conan profile update settings.compiler.libcxx=libstdc++11 default
RUN conan remote add morrigu https://tableaubit.jfrog.io/artifactory/api/conan/morrigu-conan

# Copying local files
COPY . morrigu
RUN mkdir -p morrigu/build morrigu/runtime/shaders
WORKDIR /morrigu/build

# launching build
ARG CONAN_REVISIONS_ENABLED=1
RUN conan install .. -s build_type=Debug --build=missing
RUN cmake .. -DCMAKE_BUILD_TYPE=Debug
RUN cmake --build .

# Launching the editor
# WORKDIR /morrigu/runtime
# CMD [ "/morrigu/build/bin/Macha" ]
