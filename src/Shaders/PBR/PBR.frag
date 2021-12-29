#version 450

layout(location = 0) in vec2 vs_UV;

layout(set = 3, binding = 1) uniform sampler2D u_Albedo;
layout(set = 3, binding = 2) uniform sampler2D u_Normals;

layout(location = 0) out vec4 f_Color;

void main() { f_Color = texture(u_Albedo, vs_UV); }
