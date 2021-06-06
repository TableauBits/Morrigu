#version 450

layout(location = 0) in vec2 vs_UV;

layout(set = 2, binding = 0) uniform sampler2D u_TextureA;
layout(set = 2, binding = 1) uniform sampler2D u_TextureB;

layout(location = 0) out vec4 f_Color;

void main() {
    f_Color = mix(texture(u_TextureA, vs_UV), texture(u_TextureB, vs_UV), vs_UV.x);
}
