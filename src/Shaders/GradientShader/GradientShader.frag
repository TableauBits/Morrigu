#version 450

layout(location = 0) in vec2 vs_UV;

layout(set = 2, binding = 0) uniform InterpolatedColors {
    vec3 colorA;
    vec3 colorB;
} u_Colors;

layout(location = 0) out vec4 f_Color;

void main() {
    f_Color = vec4(mix(u_Colors.colorA, u_Colors.colorB, vs_UV.y), 1);
}
