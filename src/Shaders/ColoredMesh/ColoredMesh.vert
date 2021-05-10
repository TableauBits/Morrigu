#version 450

layout(location = 0) in vec3 in_VertexPosition;
layout(location = 1) in vec3 in_VertexNormal;
layout(location = 2) in vec3 in_VertexColor;

layout(location = 0) out vec3 out_Color;

void main() {
    gl_Position = vec4(in_VertexPosition, 1.f);
    out_Color = in_VertexColor;
}
