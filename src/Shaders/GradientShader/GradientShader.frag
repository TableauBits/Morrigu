#version 450

layout(location = 0) in vec2 vs_UV;

layout(set = 2, binding = 0) uniform InterpolatedColors {
    vec4 colorA;
    vec4 colorB;
} u_Colors;

layout(location = 0) out vec4 f_Color;

void main() {
    f_Color = mix(u_Colors.colorA, u_Colors.colorB, 1 - vs_UV.y);
}
