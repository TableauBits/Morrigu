#version 450

layout(location = 0) in vec2 vs_UVPassthrough;

layout(set = 3, binding = 1) uniform sampler2D u_Bitmap;

layout(location = 0) out vec4 f_Color;

void main() { f_Color = vec4(1.0, 1.0, 1.0, texture(u_Bitmap, vs_UVPassthrough).a); }
