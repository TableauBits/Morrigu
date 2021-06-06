#version 450

layout(location = 0) in vec3 v_Position;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec2 v_UV;

layout(set = 0, binding = 0) uniform CameraData {
    mat4 view;
    mat4 projection;
    mat4 viewProjection;
} u_CameraData;

layout(set = 3, binding = 0) uniform ModelData {
    mat4 modelMatrix;
} u_ModelData;

layout(location = 0) out vec2 fs_TexCoord;

void main() {
    mat4 transform = u_CameraData.viewProjection * u_ModelData.modelMatrix;
    gl_Position = transform * vec4(v_Position, 1.f);
    fs_TexCoord = v_UV;
}
