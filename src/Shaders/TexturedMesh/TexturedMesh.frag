#version 450

layout(location = 0) in vec2 in_TexCoord;

layout(location = 0) out vec4 out_FragColor;

void main() {
    out_FragColor = vec4(in_TexCoord, 0, 1);
}
