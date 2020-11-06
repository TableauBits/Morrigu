#!/bin/bash
cd "$(dirname "$0")"

# This script calls glslc from the vulkan SDk to compile every shader module.
# The way morrigu handles shader is by having a list of "modules", each represented by a subfolder here.
# Each module must present a gl.glsl OpenGL shader, and a vk.frag as well as a vk.vert vulkan equivalent.
# This script compiles only the vulkan shaders into frag.spv and vert.spv files.
# To add a module, simply add a directory along the others, and it should be handled the same as the others
# without any other input (as long as needed files are present).

# glslc is included in the vulkan SDK. This script will look for the binary using the environment variable usually set by the official install scripts. 

for D in *; do
    if [ -d "${D}" ]; then
        "${VULKAN_SDK}/bin/glslc" "${D}/vk.vert" -o "${D}/vert.spv"
        "${VULKAN_SDK}/bin/glslc" "${D}/vk.frag" -o "${D}/frag.spv"
    fi
done