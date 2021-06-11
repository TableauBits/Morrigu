#version 450

#define TAU 6.28318530718

layout(location = 0) in vec2 vs_UVPassthrough;

layout(set = 0, binding = 1) uniform TimeData {
    vec4 timeScales;
} u_TimeData;

layout(location = 0) out vec4 f_Color;

void main() {
    float xOffset = cos(vs_UVPassthrough.y * TAU * 8) * 0.01;
    float t = cos((vs_UVPassthrough.x + xOffset + u_TimeData.timeScales.y * 0.1) * TAU * 5) * 0.5 + 0.5;

    vec4 color = mix(vec4(0, 0, 0, 1), vec4(vs_UVPassthrough.x, 1 - vs_UVPassthrough.y, 0, 1), t);

    f_Color = color;
}
