#version 450

layout(location = 0) in vec2 in_TexCoord;

layout(set = 2, binding = 0) uniform InterpolatedColors {
    vec3 colorA;
    vec3 colorB;
} in_Colors;

layout(location = 0) out vec4 out_FragColor;

void main() {
    out_FragColor = vec4(mix(in_Colors.colorA, in_Colors.colorB, in_TexCoord.x), 1);
}
