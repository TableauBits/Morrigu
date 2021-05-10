#version 450

layout(location = 0) in vec3 in_Color;

layout(location = 0) out vec4 out_FragColor;

void main() {
    out_FragColor = vec4(in_Color, 1.f);
}
