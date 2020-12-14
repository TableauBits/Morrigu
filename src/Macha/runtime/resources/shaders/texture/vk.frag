#version 450

layout(binding = 1) uniform sampler2D u_textures[32];

layout(location = 0) in vec4 v_color;
layout(location = 1) in vec2 v_texCoord;
layout(location = 2) in float v_texIndex;
layout(location = 3) in float v_tilingFactor;

layout(location = 0) out vec4 color;

void main() { color = texture(u_textures[int(v_texIndex)], v_texCoord * v_tilingFactor) * v_color; }
