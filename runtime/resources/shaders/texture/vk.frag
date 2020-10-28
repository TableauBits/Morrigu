#version 450

layout(binding = 1) uniform sampler2D u_texture;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in float v_tiling;

layout(location = 0) out vec4 outColor;

void main() { outColor = texture(u_texture, fragTexCoord * v_tiling); }