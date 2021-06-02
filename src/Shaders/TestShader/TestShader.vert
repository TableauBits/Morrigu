#version 450

layout(location = 0) in vec3 v_Position;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec2 v_UV;

layout(set = 0, binding = 0) uniform CameraData {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 viewProjectionMatrix;
} u_CameraData;

layout(push_constant) uniform PushConstant {
    mat4 modelMatrix;
} pc_Model;

layout(location = 0) out vec2 fs_UVPassThrough;

void main() {
    mat4 transform = u_CameraData.viewProjectionMatrix * pc_Model.modelMatrix;
    gl_Position = transform * vec4(v_Position, 1);
    fs_UVPassThrough = v_UV;
}
