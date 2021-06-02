#version 450

layout(location = 0) in vec2 in_TexCoord;

layout(set = 0, binding = 1) uniform TimeData {
    vec4 time;
} in_TimeData;

layout(set = 2, binding = 0) uniform sampler2D in_TextureA;
layout(set = 2, binding = 1) uniform sampler2D in_TextureB;

layout(location = 0) out vec4 out_FragColor;

void main() {
    out_FragColor = mix(texture(in_TextureA, in_TexCoord), texture(in_TextureB, in_TexCoord), in_TexCoord.x);
}
